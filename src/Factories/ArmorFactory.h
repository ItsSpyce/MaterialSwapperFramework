#pragma once

#include <fmt/format.h>

#include "IO/MaterialLoader.h"
#include "NiOverride.h"
#include "NifHelpers.h"
#include "RE/Misc.h"
#include "Save/Save.h"
#include "Singleton.h"

namespace Factories {
class ArmorFactory : public Singleton<ArmorFactory> {
 public:
  static void ResetMaterials(RE::TESObjectREFR* refr) {
    ApplyDefaultMaterial(refr, nullptr);
  }

  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectARMO* form,
                     const std::unique_ptr<MaterialConfig>& material) {
    RETURN_IF_FALSE(refr)
    RETURN_IF_FALSE(form)
    auto uid = NiOverride::GetItemUniqueID()(RE::StaticFunctionTag{}, refr, 0,
                                             (int)form->GetSlotMask(), true);
    if (uid == NULL) {
      logger::warn("Failed to get unique ID for form: {}", refr->GetFormID());
      return false;
    }
    if (!ApplyMaterialToRefr(refr, form, uid, material)) {
      logger::error(
          "Failed to apply material to reference: {}, form: {}, unique ID: {}",
          refr->GetFormID(), form->GetFormID(), uid);
      return false;
    }
    auto& record = armorMaterials_[uid];
    if (std::ranges::contains(record.appliedMaterials, material->name)) {
      logger::debug("Material {} already applied to armor with UID: {}",
                    material->name, uid);
      return true;  // Material already applied
    }
    std::vector oldMaterials(record.appliedMaterials);
    record.appliedMaterials.clear();
    for (const auto& materialName : oldMaterials) {
      // If a material with the same "applies" shapes is already applied, skip
      // it. The arrays might not match exactly, check length and if key is
      // present
      const auto appliedMaterialConfig =
          MaterialLoader::GetMaterialConfig(form->GetFormID(), materialName);
      if (!appliedMaterialConfig) {
        continue;
      }
      if (appliedMaterialConfig->applies.size() != material->applies.size()) {
        record.appliedMaterials.push_back(materialName);
        continue;
      }
      auto found = false;
      for (const auto& shapeName : material->applies | std::views::keys) {
        if (appliedMaterialConfig->applies.contains(shapeName)) {
          found = true;
          break;
        }
      }
      if (!found) {
        record.appliedMaterials.push_back(materialName);
      }
    }
    record.appliedMaterials.emplace_back(material->name.c_str());

    return true;
  }

  static bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                                   RE::TESObjectARMO*) {
    RETURN_IF_FALSE(refr)
    auto triShapes = NifHelpers::GetAllTriShapes(refr->Get3D());
    for (auto& triShape : triShapes) {
      auto material = NifHelpers::GetShaderProperty(triShape);
      if (!material) {
        continue;
      }
      if (NifHelpers::HasBuiltInMaterial(material)) {
        auto materialFile =
            MaterialLoader::LoadMaterial(material->name.c_str());
        if (!materialFile) {
          logger::error("Failed to load material file: {}",
                        material->name.c_str());
          continue;
        }
        ApplyMaterialToNode(refr, true, triShape, materialFile);
      }
    }
    return true;
  }

  bool ApplySavedMaterial(
      RE::TESObjectREFR* refr,
      const std::unique_ptr<Helpers::InventoryItem>& equippedItem) {
    RETURN_IF_FALSE(refr)
    auto actor = refr->As<RE::Actor>();
    RETURN_IF_FALSE(actor)
    auto* armo = equippedItem->data->object->As<RE::TESObjectARMO>();
    if (!armo) {
      logger::warn("Equipped item is not ARMO");
      return false;
    }
    if (equippedItem->uid == NULL) {
      ApplyDefaultMaterial(refr, armo);
      return false;
    }
    logger::debug(
        "Applying saved material to equipped item: {}, form: {}, unique "
        "ID: {}",
        refr->GetFormID(), equippedItem->data->object->GetFormID(),
        equippedItem->uid);
    auto& [appliedMaterials] = armorMaterials_[equippedItem->uid];
    logger::debug("Applying materials: {}", fmt::join(appliedMaterials, ", "));
    for (const auto& material : appliedMaterials) {
      logger::debug("Iterating over material {}", material);
      auto appliedMaterialConfig =
          MaterialLoader::GetMaterialConfig(armo->GetFormID(), material);
      if (!appliedMaterialConfig) {
        logger::debug("Material {} not found in material configs, skipping",
                      material);
        continue;
      }
      if (!ApplyMaterial(refr, armo, appliedMaterialConfig)) {
        logger::error(
            "Failed to apply material to reference: {}, form: {}, "
            "unique ID: {}",
            refr->GetFormID(), armo->GetFormID(), equippedItem->uid);
        continue;
      }
      logger::debug("Successfully applied material {}", material);
    }
    return true;
  }

  void LoadFromSave(Save::SaveData& saveData) {
    armorMaterials_.clear();
    logger::debug("Loading {} saved materials", saveData.armorRecords.size());
    for (const auto& saveRecord : saveData.armorRecords) {
      if (saveRecord.appliedMaterials.empty()) {
        logger::warn("No materials applied to armor record with UID: {}",
                     saveRecord.uid);
        continue;
      }
      logger::debug("Iterating over armor record with uid={}, materials={}",
                    saveRecord.uid,
                    fmt::join(saveRecord.appliedMaterials, ", "));
      ArmorMaterialRecord record;
      for (const auto& material : saveRecord.appliedMaterials) {
        record.appliedMaterials.emplace_back(material);
      }
      logger::debug("Added armor material record: uid={}, materials={}",
                    saveRecord.uid, fmt::join(record.appliedMaterials, ", "));
      armorMaterials_[saveRecord.uid] = std::move(record);
    }
  }

  void WriteToSave(Save::SaveData& saveData) {
    for (auto& [uid, val] : armorMaterials_) {
      if (uid == NULL) {
        logger::warn("Received NULL uid for armor record {}", uid);
      }
      if (val.appliedMaterials.empty()) {
        logger::warn("No materials applied for armor record with UID: {}", uid);
        continue;
      }
      logger::debug("Writing armor material record: uid={}, materials={}", uid,
                    fmt::join(val.appliedMaterials, ", "));
      Save::V1::ArmorRecordEntry entry{.uid = uid};
      for (const auto& material : val.appliedMaterials) {
        entry.appliedMaterials.emplace_back(material);
      }
      saveData.armorRecords.emplace_back(entry);
    }
  }

 private:
  struct ArmorMaterialRecord {
    std::vector<std::string> appliedMaterials;
  };
  std::unordered_map<int, ArmorMaterialRecord> armorMaterials_;

  static bool ApplyMaterialToRefr(
      RE::TESObjectREFR* refr, const RE::TESObjectARMO* form, int uid,
      const std::unique_ptr<MaterialConfig>& material) {
    RETURN_IF_FALSE(refr)
    RETURN_IF_FALSE(form)
    RETURN_IF_FALSE(material)
    logger::debug("Applying material to reference: {}, form: {}, unique ID: {}",
                  refr->GetFormID(), form->GetFormID(), uid);
    auto* refrModel = refr->Get3D();
    for (const auto& [shapeName, materialName] : material->applies) {
      logger::debug("Setting shape {} material to {}", shapeName, materialName);
      auto* nivAv = refrModel->GetObjectByName(shapeName);
      auto* triShape = nivAv ? nivAv->AsTriShape() : nullptr;
      if (!triShape) {
        logger::warn("No tri-shape found for shape name: {}", shapeName);
        continue;
      }
      auto materialFile = MaterialLoader::LoadMaterial(materialName);
      if (!materialFile) {
        logger::error("Failed to load material file: {}", materialName);
        continue;
      }
      ApplyMaterialToNode(refr, true, triShape, materialFile);
    }
    return true;
  }

  static void SetItemDisplayName(
      const std::unique_ptr<RE::InventoryEntryData>& data,
      const std::string& name) {
    if (!data) {
      return;
    }
    if (data->extraLists) {
      for (auto& extraList : *data->extraLists) {
        if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
          auto textDisplayData =
              extraList->GetByType<RE::ExtraTextDisplayData>();
          textDisplayData->SetName(name.c_str());
          return;
        } else {
          auto textDisplayData = new RE::ExtraTextDisplayData(name.c_str());
          extraList->Add(textDisplayData);
          return;
        }
      }
    }
  }

  static void ClearItemDisplayName(
      const std::unique_ptr<RE::InventoryEntryData>& data) {
    if (!data) {
      return;
    }
    if (data->extraLists) {
      for (auto& extraList : *data->extraLists) {
        if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
          extraList->RemoveByType(RE::ExtraDataType::kTextDisplayData);
          return;
        }
      }
    }
  }

  static std::string AppendMaterialName(RE::TESForm* form,
                                        const MaterialConfig& material) {
    std::string name(form->GetName());
    std::string nameWithoutPreviousMaterial;
    if (size_t index; StringHelpers::HasMaterialName(form, index)) {
      nameWithoutPreviousMaterial = name.substr(0, index);
    } else {
      nameWithoutPreviousMaterial = name;
    }
    return fmt::format("{} ({})", nameWithoutPreviousMaterial, material.name);
  }

  static std::optional<std::string> GetTexturePath(const std::string& in) {
    if (in.empty()) {
      logger::error("Empty texture path provided");
      return std::nullopt;
    }
    auto result = StringHelpers::PrefixTexturesPath(in);
    logger::debug("Texture path: {}", result);
    return result;
  }

  static RE::BSLightingShaderProperty* GetShaderProperty(
      RE::BSTriShape* bsTriShape) {
    auto* properties = bsTriShape->properties[1].get();
    auto* bsLightShader =
        properties ? netimmerse_cast<RE::BSLightingShaderProperty*>(properties)
                   : nullptr;
    return bsLightShader;
  }

  static RE::NiSourceTexture* LoadTexture(const char* path) {
    logger::debug("Loading texture from path: {}", path);
    auto texturePtr = RE::NiTexturePtr();
    RE::GetTexture(path, true, texturePtr, false);
    if (!texturePtr) {
      logger::error("Failed to get texture: {}", path);
      return nullptr;
    }
    auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(texturePtr.get());
    if (!newTexture) {
      logger::error("Failed to cast texture to NiSourceTexture for map: {}",
                    path);
      return nullptr;
    }
    return newTexture;
  }

  static bool ApplyMaterialToNode(
      RE::TESObjectREFR* refr, bool, RE::BSTriShape* bsTriShape,
      const std::unique_ptr<MaterialRecord>& record) {
    RETURN_IF_FALSE(refr)
    RETURN_IF_FALSE(bsTriShape)
    auto* shaderProperty = GetShaderProperty(bsTriShape);
    auto* base = skyrim_cast<RE::BSLightingShaderMaterialBase*>(
        shaderProperty->material);
    if (!base) {
      logger::error(
          "Failed to get BSLightingShaderMaterialBase for tri shape: {}",
          bsTriShape->name);
      return false;
    }

    if (!record->diffuseMap.empty()) {
      base->diffuseTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->diffuseMap.c_str()));
      return true;
    }
    if (!record->normalMap.empty()) {
      base->normalTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->normalMap.c_str()));
    }
    if (!record->smoothSpecMap.empty()) {
      base->specularBackLightingTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->smoothSpecMap.c_str()));
      base->specularPower = record->backLightPower;

      base->rimLightPower = record->rimPower;
      base->specularColor =
          RE::NiColor(record->specularColor[0], record->specularColor[1],
                      record->specularColor[2]);
    }
    base->materialAlpha = record->transparency;
    auto feature = base->GetFeature();
    if (feature == RE::BSLightingShaderMaterial::Feature::kEnvironmentMap) {
      if (auto* envMaterial =
              skyrim_cast<RE::BSLightingShaderMaterialEnvmap*>(base)) {
        if (!record->envMap.empty()) {
          envMaterial->envTexture =
              RE::NiSourceTexturePtr(LoadTexture(record->envMap.c_str()));
        }
        if (!record->envMapMask.empty()) {
          envMaterial->envMaskTexture =
              RE::NiSourceTexturePtr(LoadTexture(record->envMapMask.c_str()));
        }
        envMaterial->envMapScale = record->envMapMaskScale;
      }
    }
    if (feature == RE::BSLightingShaderMaterial::Feature::kGlowMap &&
        !record->glowMap.empty()) {
      auto* glowMaterial =
          skyrim_cast<RE::BSLightingShaderMaterialGlowmap*>(base);
      if (glowMaterial) {
        glowMaterial->glowTexture =
            RE::NiSourceTexturePtr(LoadTexture(record->glowMap.c_str()));
      }
    }
    return true;
  }
};
}  // namespace Factories