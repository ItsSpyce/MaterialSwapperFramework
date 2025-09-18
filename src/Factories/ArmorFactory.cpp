#include "ArmorFactory.h"

#include <DirectXTex.h>
#include <d3d11.h>
#include <fmt/format.h>

#include <srell.hpp>

#include "Graphics/TextureLoader.h"
#include "Helpers.h"
#include "IO/MaterialLoader.h"
#include "MaterialHelpers.h"
#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"
#include "NifHelpers.h"
#include "RE/BSTextureSetClone.h"
#include "RE/Misc.h"
#include "Save/Types.h"

using ArmorFactory = Factories::ArmorFactory;
using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using Texture = RE::BSShaderTextureSet::Texture;
using VisitControl = RE::BSVisit::BSVisitControl;

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

static void SetItemDisplayName(ArmorFactory* factory, RE::TESObjectREFR* refr,
                               UniqueID uid) {
  _TRACE(__FUNCTION__);
  return;
  auto item = Helpers::GetInventoryItemWithUID(refr, uid);
  const auto data = item ? std::move(item->data) : nullptr;
  if (!data) {
    return;
  }
  vector<const char*> filteredMaterials;
  factory->VisitAppliedMaterials(
      uid, [&](const char* name, const MaterialConfig& config) {
        if (!config.isHidden && config.modifyName) {
          filteredMaterials.emplace_back(name);
        }
        return VisitControl::kContinue;
      });

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

struct DirectXState {
  ID3D11PixelShader* pixelShader;
  ID3D11Buffer* constantBuffer;
  ID3D11ShaderResourceView* srvs[2];
  ID3D11SamplerState* sampler;
  ID3D11RenderTargetView* rtv;

  void Release() const {
    if (pixelShader) {
      pixelShader->Release();
    }
    if (constantBuffer) {
      constantBuffer->Release();
    }
    if (srvs[0]) {
      srvs[0]->Release();
    }
    if (srvs[1]) {
      srvs[1]->Release();
    }
    if (sampler) {
      sampler->Release();
    }
  }
};

static void ApplyColorToTexture(const RE::NiSourceTexturePtr& texture,
                                const RE::NiSourceTexturePtr& colorMask,
                                const MaterialRecord* material) {
  D3D11_TEXTURE2D_DESC originalDesc;
  texture->rendererTexture->texture->GetDesc(&originalDesc);
}

static bool ApplyMaterialToNode(RE::TESObjectREFR* refr,
                                RE::BSTriShape* bsTriShape,
                                const MaterialRecord* record) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(bsTriShape)
  auto* lightingShader = NifHelpers::GetShaderProperty(bsTriShape);
  auto* alphaProperty = NifHelpers::GetAlphaProperty(bsTriShape);
  auto* newMaterial = RE::BSLightingShaderMaterialBase::CreateMaterial(
      lightingShader->material->GetFeature());
  if (!newMaterial) {
    _ERROR("Failed to create BSLightingShaderMaterialBase for tri shape: {}",
           bsTriShape->name);
    return false;
  }
  newMaterial->CopyMembers(lightingShader->material);
  MaterialHelpers::ApplyFlags(lightingShader, record);

  if (record->transparency.has_value()) {
    newMaterial->materialAlpha = *record->transparency;
  }
  if (record->emitEnabled) {
    if (record->emitColor.has_value()) {
      lightingShader->emissiveColor =
          MaterialHelpers::GetColorPtr(*record->emitColor);
    }
    lightingShader->emissiveMult =
        record->emitMult.value_or(lightingShader->emissiveMult);
  }
  if (alphaProperty) {
    alphaProperty->SetAlphaBlending(
        record->alphaBlend.value_or(alphaProperty->GetAlphaBlending()));
    alphaProperty->SetAlphaTesting(
        record->alphaTest.value_or(alphaProperty->GetAlphaTesting()));
    alphaProperty->alphaThreshold =
        record->alphaTestThreshold.value_or(alphaProperty->alphaThreshold);
  }
  /*textureSet->SetTexturePath(Texture::kDiffuse, record->diffuseMap);
  textureSet->SetTexturePath(Texture::kNormal, record->normalMap);
  textureSet->SetTexturePath(Texture::kSpecular, record->specularMap);
  textureSet->SetTexturePath(Texture::kEnvironment, record->envMap);
  textureSet->SetTexturePath(Texture::kEnvironmentMask, record->envMapMask);
  textureSet->SetTexturePath(Texture::kGlowMap, record->glowMap);*/
  auto* textureLoader = Graphics::TextureLoader::GetSingleton();

  if (const auto diffuseMap =
          MaterialHelpers::GetStringPtr(record->diffuseMap)) {
    newMaterial->diffuseTexture =
        RE::NiPointer(textureLoader->LoadTexture(diffuseMap));
  }
  if (const auto normalMap = MaterialHelpers::GetStringPtr(record->normalMap)) {
    newMaterial->normalTexture =
        RE::NiPointer(textureLoader->LoadTexture(normalMap));
  }
  if (const auto specularMap =
          MaterialHelpers::GetStringPtr(record->specularMap)) {
    newMaterial->specularBackLightingTexture =
        RE::NiPointer(textureLoader->LoadTexture(specularMap));
  }
  if (record->envMapEnabled.value_or(lightingShader->flags.any(
          RE::BSShaderProperty::EShaderPropertyFlag::kEnvMap))) {
    auto* envMapMaterial =
        skyrim_cast<RE::BSLightingShaderMaterialEnvmap*>(newMaterial);
    if (const auto envMap = MaterialHelpers::GetStringPtr(record->envMap)) {
      envMapMaterial->envTexture =
          RE::NiPointer(textureLoader->LoadTexture(envMap));
    }
    if (const auto envMapMask =
            MaterialHelpers::GetStringPtr(record->envMapMask)) {
      envMapMaterial->envMaskTexture =
          RE::NiPointer(textureLoader->LoadTexture(envMapMask));
    }
    newMaterial = envMapMaterial;
  }
  if (record->glowMapEnabled.value_or(lightingShader->flags.any(
          RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap))) {
    auto* glowMapMaterial =
        skyrim_cast<RE::BSLightingShaderMaterialGlowmap*>(newMaterial);
    if (const auto glowMap = MaterialHelpers::GetStringPtr(record->glowMap)) {
      glowMapMaterial->glowTexture =
          RE::NiPointer(textureLoader->LoadTexture(glowMap));
    }
    newMaterial = glowMapMaterial;
  }

  if (record->uvOffset.has_value()) {
    newMaterial->texCoordOffset[0] =
        MaterialHelpers::GetPoint2(*record->uvOffset);
  }
  if (record->uvScale.has_value()) {
    newMaterial->texCoordScale[0] =
        MaterialHelpers::GetPoint2(*record->uvScale);
  }
  if (record->specularEnabled) {
    if (record->specularPower.has_value()) {
      newMaterial->specularPower = *record->specularPower;
    }
    newMaterial->refractionPower =
        record->refractionPower.value_or(newMaterial->refractionPower);
    newMaterial->specularColorScale =
        record->specularMult.value_or(newMaterial->specularColorScale);
    if (record->specularColor.has_value()) {
      newMaterial->specularColor =
          MaterialHelpers::GetColor(*record->specularColor);
    }
  }
  if (record->rimLighting) {
    newMaterial->rimLightPower =
        record->rimPower.value_or(newMaterial->rimLightPower);
  }

  lightingShader->SetMaterial(newMaterial, true);
  lightingShader->SetupGeometry(bsTriShape);
  lightingShader->FinishSetupGeometry(bsTriShape);
  newMaterial->~BSLightingShaderMaterialBase();
  RE::free(newMaterial);
  return true;
}

static bool ApplySavedMaterial_Impl(
    ArmorFactory* factory, RE::Actor* refr,
    const Helpers::InventoryItem* equippedItem) {
  RETURN_IF_FALSE(refr)
  auto actor = refr->As<RE::Actor>();
  RETURN_IF_FALSE(actor)
  auto* armo = equippedItem->data->object->As<RE::TESObjectARMO>();
  if (!armo) {
    _WARN("Equipped item is not ARMO");
    return false;
  }
  if (equippedItem->uid == NULL) {
    return true;
  }
  factory->VisitAppliedMaterials(
      equippedItem->uid,
      [&](const char*, const MaterialConfig& appliedMaterialConfig) {
        if (!factory->ApplyMaterial(refr, armo, &appliedMaterialConfig)) {
          _ERROR(
              "Failed to apply material to reference: {}, form: {}, "
              "unique ID: {}",
              refr->GetFormID(), armo->GetFormID(), equippedItem->uid);
        }
        return VisitControl::kContinue;
      });
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
      _WARN("No tri-shape found for shape name: {}", shapeName);
      continue;
    }
    auto materialFile = MaterialLoader::LoadMaterial(materialName);
    if (!materialFile) {
      _ERROR("Failed to load material file: {}", materialName);
      continue;
    }
    ApplyMaterialToNode(refr, triShape, materialFile);
  }
  return true;
}

void ArmorFactory::OnUpdate() {
  if (updateStack_.empty()) {
    return;  // No updates to process
  }
  auto limit = 30;
  auto count = 0;
  while (count < limit && !updateStack_.empty()) {
    count++;
    auto& [refr, armo] = updateStack_.top();
    updateStack_.pop();
    if (!refr) {
      _WARN("Invalid reference handle: 0");
      continue;
    }
    auto actor = refr->As<RE::Actor>();
    if (!actor->Is3DLoaded()) {
      updateStack_.push({.refr = refr, .armo = armo});
      continue;
    }
    if (armo) {
      auto uid = Helpers::GetUniqueID(actor, armo->GetSlotMask(), false);
      if (uid == NULL) {
        continue;
      }
      auto* arma = armo->GetArmorAddon(actor->GetRace());
      if (!arma) {
        _WARN("No armor addon found for armor: {}", armo->GetFormID());
        continue;
      }
      RE::NiPointer<RE::NiNode> armoNifPtr;
      if (RE::Demand(
              arma->bipedModels[actor->GetActorBase()->GetSex()].GetModel(),
              armoNifPtr, RE::BSModelDB::DBTraits::ArgsType{})) {
        // Apply default material if no uniqueID found
        NifHelpers::VisitShapes(armoNifPtr, [&](RE::BSTriShape* shape) {
          auto& material = shape->properties[RE::BSGeometry::States::kEffect];
          if (!material) {
            return VisitControl::kContinue;
          }
          if (std::string(material->name).ends_with(".json")) {
            auto materialFile =
                MaterialLoader::LoadMaterial(material->name.c_str());
            if (!materialFile) {
              _ERROR("Failed to load material file: {}",
                     material->name.c_str());
              return VisitControl::kContinue;
            }
            ApplyMaterialToNode(actor, shape, materialFile);
          }
          return VisitControl::kContinue;
        });
        auto item = Helpers::GetInventoryItemWithUID(actor, uid);
        if (!item) {
          _WARN("No inventory item found for uniqueID: {}", uid);
          continue;
        }
        ApplySavedMaterial_Impl(this, actor, item);
      }
    } else {
      RE::BSVisit::TraverseScenegraphObjects(
          actor->Get3D(), [&](RE::NiAVObject* geometry) {
            auto* triShape = geometry->AsTriShape();
            if (!triShape) {
              return VisitControl::kContinue;
            }
            auto& material =
                triShape->properties[RE::BSGeometry::States::kEffect];
            if (!material) {
              return VisitControl::kContinue;
            }
            if (std::string(material->name).ends_with(".json")) {
              auto materialFile =
                  MaterialLoader::LoadMaterial(material->name.c_str());
              if (!materialFile) {
                _ERROR("Failed to load material file: {}",
                       material->name.c_str());
                return VisitControl::kContinue;
              }
              ApplyMaterialToNode(actor, triShape, materialFile);
            }
            return VisitControl::kContinue;
          });
      // Apply all equipped armor
      Helpers::VisitEquippedInventoryItems(
          actor->As<RE::Actor>(), [&](const Helpers::InventoryItem* item) {
            if (item->uid != NULL) {
              ApplySavedMaterial_Impl(this, actor->As<RE::Actor>(), item);
            }
            return VisitControl::kContinue;
          });
    }

    Helpers::VisitEquippedInventoryItems(
        actor, [&](const Helpers::InventoryItem* item) {
          if (item->uid != NULL) {
            ApplySavedMaterial_Impl(this, actor->As<RE::Actor>(), item);
          }
          return VisitControl::kContinue;
        });
  }
}

bool ArmorFactory::ApplyMaterial(RE::TESObjectREFR* refr,
                                 RE::TESObjectARMO* form,
                                 const MaterialConfig* material) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(form)
  auto uid = Helpers::GetUniqueID(refr, form->GetSlotMask(), true);
  if (uid == NULL) {
    _WARN("Failed to get unique ID for form: {}", form->GetFormID());
    return false;
  }
  if (!ApplyMaterialToRefr(refr, form, uid, material)) {
    _ERROR("Failed to apply material to reference: {}, form: {}, unique ID: {}",
           refr->GetFormID(), form->GetFormID(), uid);
    return false;
  }
  if (!knownArmorMaterials_.contains(uid)) {
    knownArmorMaterials_[uid] = {};
  }
  auto& record = knownArmorMaterials_[uid];
  if (std::ranges::contains(record, material->name)) {
    return true;  // Material already applied
  }
  vector<string> newAppliedMaterials;
  for (const auto& appliedMaterialName : record) {
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
  record = move(newAppliedMaterials);
  SetItemDisplayName(this, refr, uid);

  return true;
}

bool ArmorFactory::ApplySavedMaterials(RE::TESObjectREFR* refr) {
  RETURN_IF_FALSE(refr)
  updateStack_.push({.refr = refr});
  return true;
}

bool ArmorFactory::ApplySavedMaterials(RE::TESObjectREFR* refr,
                                       RE::TESObjectARMO* armo) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(armo)
  if (!MaterialLoader::HasMaterialConfigs(armo->GetFormID())) {
    return false;
  }
  updateStack_.push(UpdateRequest{.refr = refr, .armo = armo});
  return true;
}

void ArmorFactory::ReadFromSave(SKSE::SerializationInterface* iface,
                                Save::SaveData& saveData) {
  while (!updateStack_.empty()) {
    updateStack_.pop();
  }
  for (const auto& [uniqueID, saveRecords] : saveData.armorRecords) {
    for (auto& [uid, appliedMaterials] : saveRecords) {
      if (appliedMaterials.empty()) {
        _WARN("No materials applied to armor record with UID: {}", uid);
        continue;
      }
      vector<string> validMaterials;
      for (const auto& material : appliedMaterials) {
        if (material.empty()) {
          continue;  // Skip empty material names
        }
        validMaterials.push_back(material);
      }
      if (validMaterials.empty()) {
        continue;
      }
      knownArmorMaterials_[uniqueID] = std::move(validMaterials);
    }
  }
}

void ArmorFactory::WriteToSave(SKSE::SerializationInterface* iface,
                               Save::SaveData& saveData) const {
  saveData.armorRecords.clear();
  for (auto& [uniqueID, records] : knownArmorMaterials_) {
    if (records.empty()) {
      continue;
    }
    const auto formID = UniqueIDTable::GetSingleton()->GetFormID(uniqueID);
    ArmorRecordEntryV2 record{.uniqueID = uniqueID,
                              .appliedMaterials = records};
    if (saveData.armorRecords.contains(formID)) {
      saveData.armorRecords[formID].push_back(record);
    } else {
      saveData.armorRecords[formID] = {record};
    }
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
  /*Helpers::VisitEquippedInventoryItems(actor,
                                       [&](const Helpers::InventoryItem* item) {
                                         knownArmorMaterials_.erase(item->uniqueID);
                                         ClearItemDisplayName(item->data);
                                         return VisitControl::kContinue;
                                       });*/
  updateStack_.push({.refr = refr});
}

void ArmorFactory::VisitAppliedMaterials(
    const UniqueID uid,
    const Visitor<const char*, const MaterialConfig&>& visitor) {
  if (uid == NULL) {
    return;
  }
  auto formID = UniqueIDTable::GetSingleton()->GetFormID(uid);
  auto* armo = RE::TESForm::LookupByID<RE::TESObjectARMO>(formID);
  if (!armo) {
    return;
  }
  if (auto it = knownArmorMaterials_.find(formID);
      it != knownArmorMaterials_.end()) {
    auto materials = it->second;
    for (const auto& materialName : materials) {
      if (auto* materialConfig =
              MaterialLoader::GetMaterialConfig(formID, materialName)) {
        if (visitor(materialName.c_str(), *materialConfig) ==
            VisitControl::kStop) {
          return;
        }
      }
    }
  }
}