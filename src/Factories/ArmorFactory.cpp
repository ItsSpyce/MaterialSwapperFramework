#include "ArmorFactory.h"

#include <fmt/format.h>

#include <srell.hpp>

#include "Graphics/D3DDevice.h"
#include "Graphics/ShaderResource.h"
#include "Helpers.h"
#include "Models/MaterialRecord.h"
#include "NifHelpers.h"
#include "RE/Misc.h"
#include "TintMaskBuilder.h"

using ArmorFactory = Factories::ArmorFactory;
using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;
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
                               int uid) {
  _TRACE(__FUNCTION__);
  return;
  auto item = Helpers::GetInventoryItemWithUID(refr, uid);
  const auto data = item ? std::move(item->data) : nullptr;
  if (!data) {
    return;
  }
  vector<const char*> filteredMaterials;
  factory->VisitAppliedMaterials(
      item->uid, [&](const char* name, const MaterialConfig& config) {
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

static void ApplyColorToTexture(const RE::NiSourceTexturePtr& texture,
                                const RE::NiSourceTexturePtr& colorMask,
                                const MaterialRecord* material) {
  auto* d3dTexture = texture->rendererTexture->texture;
  auto& color = material->color;
  if (!d3dTexture || !color) {
    return;
  }
  auto blendMode = material->colorBlendMode.value_or(ColorBlendMode::kMultiply);

  // Get device/context
  auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
  auto* device = (ID3D11Device*)RE::BSGraphics::Renderer::GetDevice();
  auto* context = (ID3D11DeviceContext*)renderer->GetRuntimeData().context;
  if (!device || !context) {
    return;
  }
  auto* d3dDevice = new Graphics::D3DDevice(device, context);

  // Load/compile the pixel shader
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
  Graphics::ShaderFactory shaderFactory;
  Graphics::ShaderFile* shaderFile = new Graphics::ShaderResource(
      "Data/SKSE/MaterialSwapperFramework/shaders/ColorBlend.hlsl", "main");
  shaderFactory.CreatePixelShader(d3dDevice, shaderFile, nullptr, pixelShader);

  // Set up constant buffer
  struct ColorBlendParams {
    float blendColor[3];
    int blendMode;
  } params;
  params.blendColor[0] = color->at(0);
  params.blendColor[1] = color->at(1);
  params.blendColor[2] = color->at(2);
  params.blendMode = (blendMode == ColorBlendMode::kMultiply) ? 0 : 1;

  Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
  D3D11_BUFFER_DESC cbDesc = {};
  cbDesc.ByteWidth = sizeof(ColorBlendParams);
  cbDesc.Usage = D3D11_USAGE_DEFAULT;
  cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  D3D11_SUBRESOURCE_DATA initData = {&params, 0, 0};
  device->CreateBuffer(&cbDesc, &initData, &constantBuffer);

  // Set pipeline state
  context->PSSetShader(pixelShader.Get(), nullptr, 0);
  context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

  // Create SRVs for input and mask textures
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> maskSRV;
  device->CreateShaderResourceView(d3dTexture, nullptr,
                                   inputSRV.GetAddressOf());
  device->CreateShaderResourceView(colorMask->rendererTexture->texture, nullptr,
                                   maskSRV.GetAddressOf());

  // Create a sampler state
  D3D11_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
  Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
  device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());

  // Bind input texture and sampler

  // Bind input texture and mask texture SRVs
  ID3D11ShaderResourceView* srvs[2] = {inputSRV.Get(), maskSRV.Get()};
  context->PSSetShaderResources(0, 2, srvs);

  // Bind sampler
  context->PSSetSamplers(0, 1, &sampler);
}

static RE::NiSourceTexture* LoadTexture(const char* path) {
  auto texturePtr = RE::NiTexturePtr();
  RE::GetTexture(path, true, texturePtr, false);
  if (!texturePtr) {
    _ERROR("Failed to get texture: {}", path);
    return nullptr;
  }
  auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(&*texturePtr);
  if (!newTexture) {
    _ERROR("Failed to cast texture to NiSourceTexture for map: {}", path);
    return nullptr;
  }

  return newTexture;
}

static bool ApplyMaterialToNode(RE::TESObjectREFR* refr,
                                RE::BSTriShape* bsTriShape,
                                const MaterialRecord* record) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(bsTriShape)
  auto* lightingShader = NifHelpers::GetShaderProperty(bsTriShape);
  auto* alphaProperty = NifHelpers::GetAlphaProperty(bsTriShape);
  auto* material =
      skyrim_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);
  if (!material) {
    _ERROR("Failed to get BSLightingShaderMaterialBase for tri shape: {}",
           bsTriShape->name);
    return false;
  }
  auto* newMaterial =
      RE::BSLightingShaderMaterialBase::CreateMaterial(material->GetFeature());
  if (!newMaterial) {
    _ERROR("Failed to create BSLightingShaderMaterialBase for tri shape: {}",
           bsTriShape->name);
    return false;
  }

  newMaterial->CopyMembers(material);
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
  if (record->diffuseMap.has_value() && !record->diffuseMap->empty()) {
    auto diffuseTex = RE::NiSourceTexturePtr(LoadTexture(record->diffuseMap->c_str()));
    newMaterial->diffuseTexture = diffuseTex;
    if (record->color.has_value()) {
      RE::NiSourceTexturePtr colorMask;
      if (record->colorBlendMap.has_value()) {
        colorMask = RE::NiSourceTexturePtr(LoadTexture(
            record->colorBlendMap->c_str()));
      } else {
        colorMask = nullptr;
      }
      ApplyColorToTexture(diffuseTex, colorMask, record);
    }
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
    if (record->smoothSpecMap.has_value() && !record->smoothSpecMap->empty()) {
      newMaterial->specularBackLightingTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->smoothSpecMap->c_str()));
    }
    newMaterial->specularPower = record->specularPower.value_or(
        record->backLightPower.value_or(newMaterial->specularPower));
    newMaterial->refractionPower =
        record->refractionPower.value_or(newMaterial->refractionPower);
    newMaterial->specularColorScale =
        record->specularMult.value_or(newMaterial->specularColorScale);
    if (record->specularColor.has_value()) {
      if (record->specularColor->at(0) > 1.0f) {
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
  if (lightingShader->flags.any(ShaderFlag::kEnvMap)) {
    if (auto* envMaterial =
            skyrim_cast<RE::BSLightingShaderMaterialEnvmap*>(newMaterial)) {
      if (record->envMap.has_value() && !record->envMap->empty()) {
        envMaterial->envTexture =
            RE::NiSourceTexturePtr(LoadTexture(record->envMap->c_str()));
      }
      if (record->envMapMask.has_value() && !record->envMapMask->empty()) {
        envMaterial->envMaskTexture =
            RE::NiSourceTexturePtr(LoadTexture(record->envMapMask->c_str()));
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
        skyrim_cast<RE::BSLightingShaderMaterialGlowmap*>(newMaterial);
    if (glowMaterial) {
      glowMaterial->glowTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->glowMap->c_str()));
    }
    newMaterial = glowMaterial;
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
    const std::unique_ptr<Helpers::InventoryItem>& equippedItem) {
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
      auto uid = Helpers::GetUniqueID(&*actor, armo->GetSlotMask(), false);
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
        // Apply default material if no uid found
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
          _WARN("No inventory item found for uid: {}", uid);
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
          actor->As<RE::Actor>(),
          [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
            if (item->uid != NULL) {
              ApplySavedMaterial_Impl(this, actor->As<RE::Actor>(), item);
            }
            return VisitControl::kContinue;
          });
    }

    Helpers::VisitEquippedInventoryItems(
        actor, [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
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
  auto& record = knownArmorMaterials_[uid];
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

void ArmorFactory::ReadFromSave(Save::SaveData& saveData) {
  while (!updateStack_.empty()) {
    updateStack_.pop();
  }
  for (const auto& saveRecord : saveData.armorRecords) {
    if (saveRecord.appliedMaterials.empty()) {
      _WARN("No materials applied to armor record with UID: {}",
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
    knownArmorMaterials_[saveRecord.uid] = std::move(record);
  }
}

void ArmorFactory::WriteToSave(Save::SaveData& saveData) const {
  for (auto& [uid, val] : knownArmorMaterials_) {
    if (uid == NULL) {
      _WARN("Received NULL uid for armor record {}", uid);
    }
    if (val.appliedMaterials.empty()) {
      _WARN("No materials applied for armor record with UID: {}", uid);
      continue;
    }
    ArmorRecordEntry entry{.uid = uid};
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
      actor, [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
        knownArmorMaterials_.erase(item->uid);
        ClearItemDisplayName(item->data);
        return VisitControl::kContinue;
      });
  updateStack_.push({.refr = refr});
}

void ArmorFactory::VisitAppliedMaterials(
    int uid, const Visitor<const char*, const MaterialConfig&>& visitor) {
  if (uid == NULL) {
    return;
  }
  auto* armo = Helpers::GetFormFromUniqueID(uid);
  if (!armo) {
    _TRACE("ARMO record not found for UID: {}", uid);
    knownArmorMaterials_.erase(uid);
    return;  // No armor form found for the given UID
  }
  if (auto it = knownArmorMaterials_.find(uid);
      it != knownArmorMaterials_.end()) {
    auto& [appliedMaterials] = it->second;
    for (const auto& materialName : appliedMaterials) {
      if (materialName.empty()) {
        continue;  // Skip empty material names
      }
      if (const auto* config = MaterialLoader::GetMaterialConfig(
              armo->GetFormID(), materialName)) {
        BREAK_IF_STOP(visitor, materialName.c_str(), *config);
      } else {
        _WARN("Material config not found for name: {}", materialName);
      }
    }
  }
}