#pragma once

#include <fmt/format.h>

#include "Factory.h"
#include "NiOverride.h"
#include "NifHelpers.h"
#include "RE/Misc.h"
#include "Singleton.h"

namespace Factories {
class ArmorFactory : public Factory<RE::TESObjectARMO>,
                     public Singleton<ArmorFactory> {
  struct ArmorInventoryItem {
    Helpers::InventoryItem item;
    std::vector<MaterialConfig> materials;

    _NODISCARD std::string GetMaterialName() const {
      auto materialNames = std::ranges::views::transform(
          materials, [](const MaterialConfig& mat) { return mat.name; });
      return fmt::format("{} ({})", item.data->GetDisplayName(),
                         fmt::join(materialNames, "/"));
    }
  };

 public:
  void ResetMaterials(RE::TESObjectREFR* refr) {
    ApplyDefaultMaterial(refr, nullptr);
  }

  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectARMO* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    RETURN_IF_FALSE(form)
    auto uid = NiOverride::GetItemUniqueID()(RE::StaticFunctionTag{}, refr, 0,
                                             (int)form->GetSlotMask(), true);
    if (!uid) {
      logger::warn("Failed to get unique ID for reference: {}, form: {}",
                   refr->GetFormID(), form->GetFormID());
      return false;
    }
    logger::debug("Applying material to reference: {}, form: {}, unique ID: {}",
                  refr->GetFormID(), form->GetFormID(), uid);
    auto slotMask = ConvertSlotMask(form->GetSlotMask());
    auto currentBiped = refr->GetCurrentBiped();
    if (!currentBiped) {
      logger::warn("No current biped found for reference: {}",
                   refr->GetFormID());
      return false;
    }

    auto& bipedObject = currentBiped->objects[slotMask];
    RETURN_IF_FALSE(bipedObject.item)
    auto armo = bipedObject.item->As<RE::TESObjectARMO>();
    auto inventoryItem =
        Helpers::GetInventoryItemWithFormID(refr, armo->GetFormID());
    RETURN_IF_FALSE(inventoryItem.data)
    if (!material.modifyName) {
      ClearItemDisplayName(inventoryItem.data);
    } else {
      const auto newDisplayName = AppendMaterialName(armo, material);
      SetItemDisplayName(inventoryItem.data, newDisplayName);
    }
    if (inventoryItem.data->extraLists) {
      if (auto actor = refr->As<RE::Actor>()) {
        auto actor3d = actor->Get3D();
        auto shapes = NifHelpers::GetAllTriShapes(actor3d);
        for (auto& shape : shapes) {
          if (auto it = material.applies.find(std::string{shape->name});
              it != material.applies.end()) {
            auto materialFile = MaterialLoader::LoadMaterial(it->second);
            logger::debug("Applying material to tri shape: {}", shape->name);
            ApplyMaterialToNode(refr, true, shape, *materialFile);
          }
        }
      }
    }
    return false;
  }

  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectARMO*) override {
    RETURN_IF_FALSE(refr)
    auto triShapes = NifHelpers::GetAllTriShapes(refr->Get3D());
    for (auto& triShape : triShapes) {
      auto material = NifHelpers::GetShaderProperty(triShape);
      if (!material) {
        continue;
      }
      if (NifHelpers::HasBuiltInMaterial(material)) {
        logger::debug("Applying default material to tri shape: {}",
                      triShape->name);
        auto materialFile =
            MaterialLoader::LoadMaterial(material->name.c_str());
        if (!materialFile) {
          logger::error("Failed to load material file: {}",
                        material->name.c_str());
          continue;
        }
        ApplyMaterialToNode(refr, true, triShape, *materialFile);
      }
    }
    return true;
  }

  bool ApplySavedMaterial(RE::TESObjectREFR* refr,
                          RE::TESObjectARMO*) override {
    RETURN_IF_FALSE(refr)
    auto actor = refr->As<RE::Actor>();
    RETURN_IF_FALSE(actor)
    auto equippedItems = Helpers::GetEquippedInventoryItems(refr);
    if (equippedItems.empty()) {
      logger::warn("No equipped items found for reference: {}",
                   refr->GetFormID());
      return false;
    }
    std::vector<MaterialConfig> materials{};
    for (const auto& inventoryItem : equippedItems) {
      auto formID = inventoryItem.object->GetFormID();
      auto name = inventoryItem.data->GetDisplayName();
      logger::debug("Processing item: {}, form ID: {}", name, formID);
      if (size_t matIndex = 0; StringHelpers::HasMaterialName(name, matIndex)) {
        auto materialName = std::string(name).substr(
            matIndex + 3, std::strlen(name) - (matIndex + 3) - 1);
        if (auto materialRecord =
                MaterialLoader::GetMaterial(formID, materialName)) {
          logger::debug("Found material record: {}", materialRecord->name);
          materials.push_back(*materialRecord);
        } else {
          logger::warn("No material record found for form ID: {}, name: {}",
                       formID, materialName);
        }
      }
    }
    auto triShapes = NifHelpers::GetAllTriShapes(refr->Get3D());
    for (const auto& triShape : triShapes) {
      auto material = NifHelpers::GetShaderProperty(triShape);
      if (!material) {
        continue;
      }
      auto nodeName = triShape->name.c_str();
      auto it = std::ranges::find_if(materials, [&](const MaterialConfig& mat) {
        return mat.applies.contains(nodeName);
      });
      if (it == materials.end()) {
        if (NifHelpers::HasBuiltInMaterial(material)) {
          logger::debug("Applying default material to tri shape: {}",
                        triShape->name);
          auto materialFile =
              MaterialLoader::LoadMaterial(material->name.c_str());
          if (!materialFile) {
            logger::error("Failed to load material file: {}",
                          material->name.c_str());
            continue;
          }
          ApplyMaterialToNode(
              refr, actor->GetActorBase()->GetSex() == RE::SEXES::kFemale,
              triShape, *materialFile);
        }
      } else {
        auto filename = it->applies[nodeName];
        if (filename.empty()) {
          logger::warn("No material file specified for node: {}, material: {}",
                       nodeName, it->name);
          continue;
        }
        auto materialFile = MaterialLoader::LoadMaterial(filename);
        if (!materialFile) {
          logger::error("Failed to load material file: {}", filename);
          continue;
        }
        ApplyMaterialToNode(refr, true, triShape, *materialFile);
      }
    }
    return true;
  }

  bool ApplyMaterial(RE::Actor* refr, RE::BIPED_OBJECTS::BIPED_OBJECT slot,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    auto currentBiped = refr->GetCurrentBiped();
    if (!currentBiped) {
      logger::warn("No current biped found for reference: {}",
                   refr->GetFormID());
      return false;
    }

    auto& bipedObject = currentBiped->objects[slot];
    RETURN_IF_FALSE(bipedObject.item)
    auto armo = bipedObject.item->As<RE::TESObjectARMO>();
    auto uid = NiOverride::GetItemUniqueID()(RE::StaticFunctionTag{}, refr, 0,
                                             (int)armo->GetSlotMask(), true);
    if (!uid) {
      logger::warn("Failed to get unique ID for reference: {}, form: {}",
                   refr->GetFormID(), armo->GetFormID());
      return false;
    }
    logger::debug("Applying material to reference: {}, form: {}, unique ID: {}",
                  refr->GetFormID(), armo->GetFormID(), uid);

    auto inventoryItem =
        Helpers::GetInventoryItemWithFormID(refr, armo->GetFormID());
    RETURN_IF_FALSE(inventoryItem.data)
    if (!material.modifyName) {
      ClearItemDisplayName(inventoryItem.data);
    } else {
      const auto newDisplayName = AppendMaterialName(armo, material);
      SetItemDisplayName(inventoryItem.data, newDisplayName);
    }
    if (inventoryItem.data->extraLists) {
      if (auto actor = refr->As<RE::Actor>()) {
        auto actor3d = actor->Get3D();
        auto shapes = NifHelpers::GetAllTriShapes(actor3d);
        for (auto& shape : shapes) {
          if (auto it = material.applies.find(std::string{shape->name});
              it != material.applies.end()) {
            auto materialFile = MaterialLoader::LoadMaterial(it->second);
            logger::debug("Applying material to tri shape: {}", shape->name);
            ApplyMaterialToNode(
                refr, actor->GetActorBase()->GetSex() == RE::SEXES::kFemale,
                shape, *materialFile);
          }
        }
      }
    }
    return false;
  }

 private:
  static constexpr RE::BIPED_OBJECTS::BIPED_OBJECT ConvertSlotMask(
      const RE::BGSBipedObjectForm::BipedObjectSlot slot) {
    // this is a lot easier to do in the inverse direction...
    switch (slot) {
      case RE::BGSBipedObjectForm::BipedObjectSlot::kHead:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kHead;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kHair:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kHair;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kBody:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kBody;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kHands:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kHands;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kForearms:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kForearms;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kAmulet:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kAmulet;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kRing:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kRing;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kFeet:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kFeet;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kCalves:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kCalves;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kShield:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kShield;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kTail:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kTail;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kLongHair;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kCirclet;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kEars:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kEars;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModMouth:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModMouth;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModNeck:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModNeck;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModChestPrimary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModChestPrimary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModBack:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModBack;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc1:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModMisc1;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisPrimary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModPelvisPrimary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitateHead:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kDecapitateHead;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitate:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kDecapitate;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisSecondary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModPelvisSecondary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModLegRight:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModLegRight;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModLegLeft:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModLegLeft;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModFaceJewelry:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModFaceJewelry;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModChestSecondary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModChestSecondary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModShoulder:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModShoulder;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModArmLeft:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModArmLeft;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModArmRight:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModArmRight;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc2:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModMisc2;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kFX01:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kFX01;
      default:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kNone;
    }
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
    return fmt::format("{} (*{})", nameWithoutPreviousMaterial, material.name);
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
    auto texturePtr = RE::NiTexturePtr();
    RE::GetTexture(path, true, texturePtr, false);
    if (!texturePtr) {
      logger::error("Failed to get texture: {}", path);
      return nullptr;
    }
    auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(texturePtr.get());
    if (!newTexture) {
      logger::error(
          "Failed to cast texture to NiSourceTexture for diffuse map: {}",
          path);
      return nullptr;
    }
    return newTexture;
  }
  static bool ApplyMaterialToNode(RE::TESObjectREFR* refr, bool isFemale,
                                  RE::BSTriShape* bsTriShape,
                                  const MaterialRecord& record) {
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

    if (!record.diffuseMap.empty()) {
      base->diffuseTexture =
          RE::NiSourceTexturePtr(LoadTexture(record.diffuseMap.c_str()));
    }
    if (!record.normalMap.empty()) {
      base->normalTexture =
          RE::NiSourceTexturePtr(LoadTexture(record.normalMap.c_str()));
    }
    if (!record.smoothSpecMap.empty()) {
      base->specularBackLightingTexture =
          RE::NiSourceTexturePtr(LoadTexture(record.smoothSpecMap.c_str()));
      base->specularPower = record.backLightPower;

      base->rimLightPower = record.rimPower;
      base->specularColor =
          RE::NiColor(record.specularColor[0], record.specularColor[1],
                      record.specularColor[2]);
    }
    base->materialAlpha = record.transparency;
    auto feature = base->GetFeature();
    if (feature == RE::BSLightingShaderMaterial::Feature::kEnvironmentMap) {
      if (auto envMaterial =
              skyrim_cast<RE::BSLightingShaderMaterialEnvmap*>(base)) {
        if (!record.envMap.empty()) {
          envMaterial->envTexture =
              RE::NiSourceTexturePtr(LoadTexture(record.envMap.c_str()));
        }
        if (!record.envMapMask.empty()) {
          envMaterial->envMaskTexture =
              RE::NiSourceTexturePtr(LoadTexture(record.envMapMask.c_str()));
        }
        envMaterial->envMapScale = record.envMapMaskScale;
      }
    }
    if (feature == RE::BSLightingShaderMaterial::Feature::kGlowMap &&
        !record.glowMap.empty()) {
      auto glowMaterial =
          skyrim_cast<RE::BSLightingShaderMaterialGlowmap*>(base);
      if (glowMaterial) {
        glowMaterial->glowTexture =
            RE::NiSourceTexturePtr(LoadTexture(record.glowMap.c_str()));
      }
    }
    return true;
  }

  // static bool ApplyMaterialToNode(RE::TESObjectREFR* refr, bool isFemale,
  //                                 RE::BSTriShape* bsTriShape,
  //                                 const MaterialRecord& material) {
  //   RETURN_IF_FALSE(refr)
  //   RETURN_IF_FALSE(bsTriShape)
  //  auto node = bsTriShape->name.c_str();

  //  NiOverride::AddNodeOverrideFloat()(RE::StaticFunctionTag{}, refr,
  //  isFemale, node,
  //                                     NiOverride::kNiOverrideKey_ShaderAlpha,
  //                                     0, material.transparency, false);

  //  if (auto diffuseTex = GetTexturePath(material.diffuseMap)) {
  //    NiOverride::AddNodeOverrideString()(
  //        RE::StaticFunctionTag{}, refr, isFemale, node,
  //        NiOverride::kNiOverrideKey_ShaderTexture,
  //        NiOverride::kNiOverrideTex_Diffuse, diffuseTex->c_str(), false);
  //  }

  //  if (auto normalTex = GetTexturePath(material.normalMap)) {
  //    NiOverride::AddNodeOverrideString()(
  //        RE::StaticFunctionTag{}, refr, isFemale, node,
  //        NiOverride::kNiOverrideKey_ShaderTexture,
  //        NiOverride::kNiOverrideTex_Normal, normalTex->c_str(), false);
  //  }
  //  if (auto glowMap = GetTexturePath(material.glowMap)) {
  //    if (material.glowMapEnabled) {
  //      NiOverride::AddNodeOverrideString()(
  //          RE::StaticFunctionTag{}, refr, isFemale, node,
  //          NiOverride::kNiOverrideKey_ShaderTexture,
  //          NiOverride::kNiOverrideTex_Glow, glowMap->c_str(), false);
  //    }
  //  }
  //  if (auto parallaxTex = GetTexturePath(material.displacementMap)) {
  //    // I think that's what parallax is lol
  //    NiOverride::AddNodeOverrideString()(
  //        RE::StaticFunctionTag{}, refr, isFemale, node,
  //        NiOverride::kNiOverrideKey_ShaderTexture,
  //        NiOverride::kNiOverrideTex_Parallax, parallaxTex->c_str(), false);
  //  }
  //  if (auto environmentTex = GetTexturePath(material.envMap)) {
  //    NiOverride::AddNodeOverrideString()(
  //        RE::StaticFunctionTag{}, refr, isFemale, node,
  //        NiOverride::kNiOverrideKey_ShaderTexture,
  //        NiOverride::kNiOverrideTex_Environment, environmentTex->c_str(),
  //        false);
  //  }
  //  if (auto envMaskTex = GetTexturePath(material.envMapMask)) {
  //    NiOverride::AddNodeOverrideString()(
  //        RE::StaticFunctionTag{}, refr, isFemale, node,
  //        NiOverride::kNiOverrideKey_ShaderTexture,
  //        NiOverride::kNiOverrideTex_EnvMask, envMaskTex->c_str(), false);
  //  }
  //  if (auto specularTex = GetTexturePath(material.specularMap)) {
  //    NiOverride::AddNodeOverrideString()(
  //        RE::StaticFunctionTag{}, refr, isFemale, node,
  //        NiOverride::kNiOverrideKey_ShaderTexture,
  //        NiOverride::kNiOverrideTex_Specular, specularTex->c_str(), false);
  //  }
  //  NiOverride::AddNodeOverrideFloat()(
  //      RE::StaticFunctionTag{}, refr, isFemale, node,
  //      NiOverride::kNiOverrideKey_ShaderSpecularStrength, 1,
  //      material.specularMult, false);
  //  return true;
  //}
};
}  // namespace Factories