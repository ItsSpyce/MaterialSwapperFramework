#include "ArmorFactory.h"

#include <fmt/format.h>

#include <srell.hpp>

#include "NiOverride.h"
#include "NifHelpers.h"
#include "RE/Misc.h"

using ArmorFactory = Factories::ArmorFactory;
using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;
using Texture = RE::BSShaderTextureSet::Texture;

namespace TEXTURE {
static void SanitizePath(std::string& path) {
  std::ranges::transform(path, path.begin(), [](char c) {
    return static_cast<char>(std::tolower(c));
  });
  path = srell::regex_replace(path, srell::regex("/+|\\\\+"), "\\");
  path = srell::regex_replace(path, srell::regex("^\\\\+"), "");
  path = srell::regex_replace(
      path,
      srell::regex(R"(.*?[^\s]textures\\|^textures\\)", srell::regex::icase),
      "");
}

static RE::BSShaderTextureSet* CreateTextureSet(char** value) {
  const auto textureSet = RE::BSShaderTextureSet::Create();
  if (textureSet) {
    for (const auto type :
         stl::enum_range(Texture::kDiffuse, Texture::kTotal)) {
      if (strlen(value[type]) > 0) {
        textureSet->SetTexturePath(type, value[type]);
      }
    }
  }
  return textureSet;
}
}  // namespace TEXTURE

struct ArmorMaterialRecord {
  vector<string> appliedMaterials;
};

static inline unordered_map<int, ArmorMaterialRecord> g_armorMaterials;
static inline stack<RE::ObjectRefHandle> g_updateStack{};

static void ClearItemDisplayName(
    const unique_ptr<RE::InventoryEntryData>& data) {
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

static void SetItemDisplayName(RE::TESObjectREFR* refr, int uid) {
  auto item = Helpers::GetInventoryItemWithUID(refr, uid);
  auto formID = NiOverride::GetFormFromUniqueID()(RE::StaticFunctionTag{}, uid);
  const auto data = item ? std::move(item->data) : nullptr;
  if (!data) {
    return;
  }
  auto appliedMaterials = g_armorMaterials[item->uid];
  vector<const char*> filteredMaterials;
  for (auto& mat : appliedMaterials.appliedMaterials) {
    if (mat.empty()) {
      continue;
    }
    if (auto config = MaterialLoader::GetMaterialConfig(formID, mat);
        config && !config->isHidden && config->modifyName) {
      filteredMaterials.emplace_back(mat.c_str());
    }
  }
  if (filteredMaterials.empty()) {
    ClearItemDisplayName(data);
    return;
  }
  const auto name = fmt::format("{} [{}]", item->object->GetName(),
                                fmt::join(filteredMaterials, ", "));
  if (data->extraLists) {
    for (auto& extraList : *data->extraLists) {
      if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
        auto textDisplayData = extraList->GetByType<RE::ExtraTextDisplayData>();

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

static RE::NiSourceTexture* LoadTexture(const char* path) {
  auto texturePtr = RE::NiTexturePtr();
  RE::GetTexture(StringHelpers::ToLower(path).c_str(), true, texturePtr, false);
  if (!texturePtr) {
    logger::error("Failed to get texture: {}", path);
    return nullptr;
  }
  auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(&*texturePtr);
  if (!newTexture) {
    logger::error("Failed to cast texture to NiSourceTexture for map: {}",
                  path);
    return nullptr;
  }
  return newTexture;
}

static bool ApplyMaterialToNode(RE::TESObjectREFR* refr,
                                RE::BSTriShape* bsTriShape,
                                const MaterialRecord* record) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(bsTriShape)
  const auto& effect = bsTriShape->properties[RE::BSGeometry::States::kEffect];
  const auto& property =
      bsTriShape->properties[RE::BSGeometry::States::kProperty];
  auto* lightingShader =
      netimmerse_cast<RE::BSLightingShaderProperty*>(effect.get());
  auto* alphaProperty = netimmerse_cast<RE::NiAlphaProperty*>(property.get());
  const auto material = lightingShader
                            ? static_cast<RE::BSLightingShaderMaterialBase*>(
                                  lightingShader->material)
                            : nullptr;

  if (!lightingShader || !material) {
    logger::error(
        "Failed to get BSLightingShaderMaterialBase for tri shape: {}",
        bsTriShape->name);
    return false;
  }
  lightingShader->SetFlags(
      ShaderFlag8::kDecal,
      record->decal.value_or(lightingShader->flags.any(ShaderFlag::kDecal)));
  lightingShader->SetFlags(ShaderFlag8::kTwoSided,
                           record->twoSided.value_or(lightingShader->flags.any(
                               ShaderFlag::kTwoSided)));
  if (record->emitEnabled) {
    if (record->emitColor.has_value()) {
      lightingShader->emissiveColor =
          new RE::NiColor(record->emitColor->at(0), record->emitColor->at(1),
                          record->emitColor->at(2));
    }
    lightingShader->emissiveMult =
        record->emitMult.value_or(lightingShader->emissiveMult);
    lightingShader->SetFlags(ShaderFlag8::kOwnEmit, true);
  }
  if (alphaProperty) {
    alphaProperty->SetAlphaBlending(
        record->alphaBlend.value_or(alphaProperty->GetAlphaBlending()));
    alphaProperty->SetAlphaTesting(
        record->alphaTest.value_or(alphaProperty->GetAlphaTesting()));
    alphaProperty->alphaThreshold =
        record->alphaTestThreshold.value_or(alphaProperty->alphaThreshold);
  }
  if (auto newMaterial =
          static_cast<RE::BSLightingShaderMaterialBase*>(material->Create())) {
    newMaterial->CopyMembers(material);
    const auto textureSet =
        material ? material->textureSet : RE::NiPointer<RE::BSTextureSet>();

    if (textureSet) {
#define PREFIX_TEXTURES_PATH(_PATH) \
  (std::string("textures\\") + (_PATH)).c_str()

      if (const auto newTextureSet = RE::BSShaderTextureSet::Create()) {
        for (auto slot : stl::enum_range(Texture::kDiffuse,
                                             Texture::kTotal)) {
          textureSet->SetTexturePath(slot, "\0");
        }
        if (!record->diffuseMap->empty()) {
          newTextureSet->SetTexturePath(
              Texture::kDiffuse,
              PREFIX_TEXTURES_PATH(record->diffuseMap.value()));
        }
        if (!record->normalMap->empty()) {
          newTextureSet->SetTexturePath(
              Texture::kNormal,
              PREFIX_TEXTURES_PATH(record->normalMap.value()));
        }
        if (record->specularEnabled) {
          if (!record->smoothSpecMap->empty()) {
            newTextureSet->SetTexturePath(
                Texture::kBacklightMask,
                PREFIX_TEXTURES_PATH(record->smoothSpecMap.value()));
          }
        }
        newMaterial->OnLoadTextureSet(0, newTextureSet);
      }
      if (record->uvOffset.has_value()) {
        newMaterial->texCoordOffset[0] =
            RE::NiPoint2(record->uvOffset->at(0), record->uvOffset->at(0));
      }
      if (record->uvScale.has_value()) {
        newMaterial->texCoordScale[0] =
            RE::NiPoint2(record->uvScale->at(0), record->uvScale->at(1));
      }
      if (record->normalMap.has_value() && !record->normalMap->empty()) {
        newMaterial->normalTexture =
            RE::NiSourceTexturePtr(LoadTexture(record->normalMap->c_str()));
      }

      if (record->specularEnabled) {
        if (record->smoothSpecMap.has_value() &&
            !record->smoothSpecMap->empty()) {
          newMaterial->specularBackLightingTexture = RE::NiSourceTexturePtr(
              LoadTexture(record->smoothSpecMap->c_str()));
        }
        newMaterial->specularPower = record->specularPower.value_or(
            record->backLightPower.value_or(newMaterial->specularPower));
        newMaterial->refractionPower =
            record->refractionPower.value_or(newMaterial->refractionPower);
        newMaterial->specularColorScale =
            record->specularMult.value_or(newMaterial->specularColorScale);
        if (record->specularColor.has_value()) {
          if (record->specularColor->at(0) > 1.0f) {
            // we're trying to use byte represented values of a color, convert
            // to 0 to 1 float range
            newMaterial->specularColor =
                RE::NiColor(record->specularColor->at(0) / 255.0f,
                            record->specularColor->at(1) / 255.0f,
                            record->specularColor->at(2) / 255.0f);
          }
        }

      }
      if (record->rimLighting) {
        newMaterial->rimLightPower =
            record->rimPower.value_or(newMaterial->rimLightPower);
      }

      newMaterial->materialAlpha =
          record->transparency.value_or(newMaterial->materialAlpha);
      // TODO: instead of comparing features, let properties decide and clone if
      // needed
      if (lightingShader->flags.any(ShaderFlag::kEnvMap)) {
        if (auto* envMaterial =
                dynamic_cast<RE::BSLightingShaderMaterialEnvmap*>(
                    newMaterial)) {
          if (record->envMap.has_value() && !record->envMap->empty()) {
            envMaterial->envTexture =
                RE::NiSourceTexturePtr(LoadTexture(record->envMap->c_str()));
          }
          if (record->envMapMask.has_value() && !record->envMapMask->empty()) {
            envMaterial->envMaskTexture = RE::NiSourceTexturePtr(
                LoadTexture(record->envMapMask->c_str()));
          }
          envMaterial->envMapScale =
              record->envMapMaskScale.value_or(envMaterial->envMapScale);
          newMaterial = envMaterial;
        }
      }
      if (lightingShader->flags.any(ShaderFlag::kGlowMap) &&
          record->glowMapEnabled && record->glowMap.has_value() &&
          !record->glowMap->empty()) {
        auto* glowMaterial =
            dynamic_cast<RE::BSLightingShaderMaterialGlowmap*>(newMaterial);
        if (glowMaterial) {
          glowMaterial->glowTexture =
              RE::NiSourceTexturePtr(LoadTexture(record->glowMap->c_str()));
        }
        newMaterial = glowMaterial;
      }
    }

    lightingShader->SetMaterial(newMaterial, true);
    lightingShader->SetupGeometry(bsTriShape);
    lightingShader->FinishSetupGeometry(bsTriShape);
    newMaterial->~BSLightingShaderMaterialBase();
    RE::free(newMaterial);
    return true;
  }
  return false;
}

static bool ApplySavedMaterial_Impl(
    ArmorFactory* factory, RE::Actor* refr,
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
    return true;
  }
  auto& [appliedMaterials] = g_armorMaterials[equippedItem->uid];
  for (const auto& material : appliedMaterials) {
    auto appliedMaterialConfig =
        MaterialLoader::GetMaterialConfig(armo->GetFormID(), material);
    if (!appliedMaterialConfig) {
      continue;
    }
    if (!factory->ApplyMaterial(refr, armo, appliedMaterialConfig)) {
      logger::error(
          "Failed to apply material to reference: {}, form: {}, "
          "unique ID: {}",
          refr->GetFormID(), armo->GetFormID(), equippedItem->uid);
      continue;
    }
  }
  return true;
}

static bool ApplyMaterialToRefr(RE::TESObjectREFR* refr,
                                const RE::TESObjectARMO* form, int uid,
                                const MaterialConfig* material) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(form)
  RETURN_IF_FALSE(material)
  auto* refrModel = refr->Get3D();
  for (const auto& [shapeName, materialName] : material->applies) {
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
    ApplyMaterialToNode(refr, triShape, materialFile);
  }
  return true;
}

void ArmorFactory::OnUpdate() {
  if (g_updateStack.empty()) {
    return;  // No updates to process
  }
  auto& updateTask = g_updateStack.top();
  auto refr = RE::TESObjectREFR::LookupByHandle(updateTask.native_handle());
  if (!refr) {
    logger::warn("Reference not found for handle: {}",
                 updateTask.native_handle());
    g_updateStack.pop();
    return;
  }
  if (!refr->Is3DLoaded()) {
    logger::warn("Skipping while 3D is unloaded");
    return;
  }
  NifHelpers::VisitShapes(refr->Get3D(), [&](RE::BSTriShape* triShape) {
    if (!triShape) {
      return;
    }
    auto& material = triShape->properties[RE::BSGeometry::States::kEffect];
    if (!material) {
      return;
    }
    if (std::string(material->name).ends_with(".json")) {
      auto materialFile = MaterialLoader::LoadMaterial(material->name.c_str());
      if (!materialFile) {
        logger::error("Failed to load material file: {}", material->name.c_str());
        return;
      }
      ApplyMaterialToNode(refr.get(), triShape, materialFile);
    }
  });
  Helpers::VisitEquippedInventoryItems(
      refr.get(), [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
        if (item->uid != NULL) {
          ApplySavedMaterial_Impl(this, refr->As<RE::Actor>(), item);
        }
      });

  g_updateStack.pop();
}

bool ArmorFactory::ApplyMaterial(RE::Actor* refr, RE::TESObjectARMO* form,
                                 const MaterialConfig* material) {
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
  auto& record = g_armorMaterials[uid];
  if (std::ranges::contains(record.appliedMaterials, material->name)) {
    return true;  // Material already applied
  }
  std::vector<std::string> newAppliedMaterials;
  for (const auto& appliedMaterialName : record.appliedMaterials) {
    auto appliedMaterialConfig = MaterialLoader::GetMaterialConfig(
        form->GetFormID(), appliedMaterialName);
    if (!appliedMaterialConfig) {
      continue;  // Skip if material config is not found
    }
    if (appliedMaterialConfig->name == material->name) {
      continue;  // Skip if the material is already applied
    }
    // check if there's an intersection between the two shape collections
    bool hasIntersection = false;
    for (const auto& shapeName :
         appliedMaterialConfig->applies | std::views::keys) {
      if (material->applies.contains(shapeName) &&
          material->layer == appliedMaterialConfig->layer) {
        hasIntersection = true;
        break;  // Stop checking if we found an intersection
      }
    }
    if (hasIntersection) {
      continue;  // Skip if there's an intersection
    }
    newAppliedMaterials.emplace_back(appliedMaterialName);
  }
  newAppliedMaterials.emplace_back(material->name);
  record.appliedMaterials = std::move(newAppliedMaterials);
  SetItemDisplayName(refr, uid);

  return true;
}

bool ArmorFactory::ApplySavedMaterials(RE::Actor* refr) {
  RETURN_IF_FALSE(refr)
  g_updateStack.emplace(refr->GetHandle());
  return true;
}

void ArmorFactory::LoadFromSave(Save::SaveData& saveData) {
  g_updateStack = {};
  g_armorMaterials.clear();
  for (const auto& saveRecord : saveData.armorRecords) {
    if (saveRecord.appliedMaterials.empty()) {
      logger::warn("No materials applied to armor record with UID: {}",
                   saveRecord.uid);
      continue;
    }
    ArmorMaterialRecord record;
    for (const auto& material : saveRecord.appliedMaterials) {
      if (material.empty()) {
        continue;  // Skip empty material names
      }
      record.appliedMaterials.emplace_back(material);
    }
    g_armorMaterials[saveRecord.uid] = std::move(record);
  }
}

void ArmorFactory::WriteToSave(Save::SaveData& saveData) {
  for (auto& [uid, val] : g_armorMaterials) {
    if (uid == NULL) {
      logger::warn("Received NULL uid for armor record {}", uid);
    }
    if (val.appliedMaterials.empty()) {
      logger::warn("No materials applied for armor record with UID: {}", uid);
      continue;
    }
    Save::V1::ArmorRecordEntry entry{.uid = uid};
    for (const auto& material : val.appliedMaterials) {
      entry.appliedMaterials.emplace_back(material);
    }
    saveData.armorRecords.emplace_back(entry);
  }
}

void ArmorFactory::ResetMaterials(RE::TESObjectREFR* refr) {
  if (!refr) {
    return;
  }
  auto* actor = refr->As<RE::Actor>();
  if (!actor) {
    return;
  }
  Helpers::VisitEquippedInventoryItems(
      actor, [&](std::unique_ptr<Helpers::InventoryItem>& item) {
        g_armorMaterials.erase(item->uid);
        ClearItemDisplayName(item->data);
      });
  g_updateStack.push(refr->GetHandle());
}