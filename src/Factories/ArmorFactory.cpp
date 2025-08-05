#include "ArmorFactory.h"

#include <fmt/format.h>

#include "NiOverride.h"
#include "NifHelpers.h"
#include "RE/Misc.h"

using ArmorFactory = Factories::ArmorFactory;

struct ArmorMaterialRecord {
  std::vector<std::string> appliedMaterials;
};

static inline std::unordered_map<int, ArmorMaterialRecord> g_armorMaterials;
static inline std::stack<RE::ObjectRefHandle> g_updateStack{};

static void SetItemDisplayName(
    const std::unique_ptr<RE::InventoryEntryData>& data,
    const std::string& name) {
  if (!data) {
    return;
  }
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

static RE::NiSourceTexture* LoadTexture(const char* path) {
  logger::debug("Loading texture: {}", path);
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

static bool ApplyMaterialToNode(RE::TESObjectREFR* refr, bool,
                                RE::BSTriShape* bsTriShape,
                                const std::unique_ptr<MaterialRecord>& record) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(bsTriShape)
  auto* shaderProperty = NifHelpers::GetShaderProperty(bsTriShape);
  auto* alphaProperty = NifHelpers::GetAlphaProperty(bsTriShape);
  auto* oldBase =
      skyrim_cast<RE::BSLightingShaderMaterialBase*>(shaderProperty->material);
  if (!oldBase) {
    logger::error(
        "Failed to get BSLightingShaderMaterialBase for tri shape: {}",
        bsTriShape->name);
    return false;
  }
  auto* base =
      RE::BSLightingShaderMaterialBase::CreateMaterial(oldBase->GetFeature());
  if (!base) {
    logger::error(
        "Failed to create BSLightingShaderMaterialBase for tri shape: {}",
        bsTriShape->name);
    return false;
  }
  base->CopyMembers(oldBase);
  if (record->decal) {
    shaderProperty->SetFlags(RE::BSShaderProperty::EShaderPropertyFlag8::kDecal, true);
  }
  if (record->emitEnabled) {
    if (record->emitColor.has_value()) {
      shaderProperty->emissiveColor =
          new RE::NiColor(record->emitColor->at(0), record->emitColor->at(1),
                          record->emitColor->at(2));
    }
    shaderProperty->emissiveMult =
        record->emitMult.value_or(shaderProperty->emissiveMult);
    shaderProperty->SetFlags(RE::BSShaderProperty::EShaderPropertyFlag8::kOwnEmit, true);
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
    base->diffuseTexture =
        RE::NiSourceTexturePtr(LoadTexture(record->diffuseMap->c_str()));
  }
  if (record->uvOffset.has_value()) {
    base->texCoordOffset[0] =
        RE::NiPoint2(record->uvOffset->at(0), record->uvOffset->at(0));
  }
  if (record->uvScale.has_value()) {
    base->texCoordScale[0] =
        RE::NiPoint2(record->uvScale->at(0), record->uvScale->at(1));
  }
  if (record->normalMap.has_value() && !record->normalMap->empty()) {
    base->normalTexture =
        RE::NiSourceTexturePtr(LoadTexture(record->normalMap->c_str()));
  }

  if (record->specularEnabled) {
    if (record->specularMap.has_value() && !record->specularMap->empty()) {
      base->specularBackLightingTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->specularMap->c_str()));
    }
    base->specularPower = record->backLightPower.value_or(base->specularPower);
    base->refractionPower =
        record->refractionPower.value_or(base->refractionPower);
    base->specularColorScale =
        record->specularMult.value_or(base->specularColorScale);
    base->rimLightPower = record->rimPower.value_or(base->rimLightPower);
    if (record->specularColor.has_value()) {
      if (record->specularColor->at(0) > 1.0f) {
        // we're trying to use byte represented values of a color, convert to 0
        // to 1 float range
        base->specularColor =
            RE::NiColor(record->specularColor->at(0) / 255.0f,
                        record->specularColor->at(1) / 255.0f,
                        record->specularColor->at(2) / 255.0f);
      } else {
        base->specularColor = RE::NiColor(record->specularColor->at(0),
                                          record->specularColor->at(1),
                                          record->specularColor->at(2));
      }
    }

  } else {
    base->specularBackLightingTexture = nullptr;
    base->specularPower = 20.0f;
    base->refractionPower = 0.0f;
    base->specularColorScale = 1.0f;
    base->rimLightPower = 0.0f;
    base->specularColor = RE::NiColor(1.0f, 1.0f, 1.0f);
  }

  base->materialAlpha = record->transparency.value_or(base->materialAlpha);
  // TODO: instead of comparing features, let properties decide and clone if
  // needed
  const auto feature = base->GetFeature();
  if (feature == RE::BSLightingShaderMaterial::Feature::kEnvironmentMap) {
    if (auto* envMaterial =
            skyrim_cast<RE::BSLightingShaderMaterialEnvmap*>(base)) {
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
    }
  }
  if (feature == RE::BSLightingShaderMaterial::Feature::kGlowMap &&
      record->glowMapEnabled && record->glowMap.has_value() &&
      !record->glowMap->empty()) {
    auto* glowMaterial =
        skyrim_cast<RE::BSLightingShaderMaterialGlowmap*>(base);
    if (glowMaterial) {
      glowMaterial->glowTexture =
          RE::NiSourceTexturePtr(LoadTexture(record->glowMap->c_str()));
    }
  }
  shaderProperty->SetMaterial(base, true);
  return true;
}

static bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                                 RE::NiAVObject* niObject) {
  RETURN_IF_FALSE(refr)
  for (auto triShapes = NifHelpers::GetAllTriShapes(niObject);
       auto& triShape : triShapes) {
    auto material = NifHelpers::GetShaderProperty(triShape);
    if (!material) {
      continue;
    }
    if (NifHelpers::HasBuiltInMaterial(material)) {
      auto materialFile = MaterialLoader::LoadMaterial(material->name.c_str());
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
    ApplyDefaultMaterial(refr, refr->Get3D());
    return false;
  }
  auto& [appliedMaterials] = g_armorMaterials[equippedItem->uid];
  logger::debug("Applying materials: {}", fmt::join(appliedMaterials, ", "));
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

static bool ApplyMaterialToRefr(
    RE::TESObjectREFR* refr, const RE::TESObjectARMO* form, int uid,
    const std::unique_ptr<MaterialConfig>& material) {
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
    ApplyMaterialToNode(refr, true, triShape, materialFile);
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
    logger::debug("Skipping while 3D is unloaded");
    return;
  }
  logger::debug("Updating materials for form ID {}", refr->GetFormID());
  NifHelpers::VisitShapes(refr->Get3D(), [&](RE::BSTriShape* shape) {
    ApplyDefaultMaterial(refr.get(), shape);
  });
  Helpers::VisitEquippedInventoryItems(
      refr.get(), [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
        if (item->uid != NULL) {
          ApplySavedMaterial_Impl(this, refr->As<RE::Actor>(), item);
        }
      });

  g_updateStack.pop();
}

bool ArmorFactory::ApplyMaterial(
    RE::Actor* refr, RE::TESObjectARMO* form,
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
  auto& record = g_armorMaterials[uid];
  if (std::ranges::contains(record.appliedMaterials, material->name)) {
    logger::debug("Material {} already applied to armor with UID: {}",
                  material->name, uid);
    return true;  // Material already applied
  }
  std::vector<std::string> newAppliedMaterials;
  for (const auto& appliedMaterialName : record.appliedMaterials) {
    auto appliedMaterialConfig =
        MaterialLoader::GetMaterialConfig(form->GetFormID(), appliedMaterialName);
    if (!appliedMaterialConfig) {
      continue;  // Skip if material config is not found
    }
    if (appliedMaterialConfig->name == material->name) {
      logger::debug("Skipping already applied material: {}", material->name);
      continue;  // Skip if the material is already applied
    }
    // check if there's an intersection between the two shape collections
    bool hasIntersection = false;
    for (const auto& shapeName :
         appliedMaterialConfig->applies | std::views::keys) {
      if (material->applies.contains(shapeName) && material->layer == appliedMaterialConfig->layer) {
        hasIntersection = true;
        break;  // Stop checking if we found an intersection
      }
    }
    if (hasIntersection) {
      logger::debug("Skipping material {} as it intersects with {}",
                    material->name, appliedMaterialName);
      continue;  // Skip if there's an intersection
    }
    newAppliedMaterials.emplace_back(appliedMaterialName);
  }
  newAppliedMaterials.emplace_back(material->name);
  record.appliedMaterials = std::move(newAppliedMaterials);
  logger::debug("Applied materials for armor with UID: {}: {}", uid,
                fmt::join(record.appliedMaterials, ", "));

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
  logger::debug("Loading {} saved materials", saveData.armorRecords.size());
  for (const auto& saveRecord : saveData.armorRecords) {
    if (saveRecord.appliedMaterials.empty()) {
      logger::warn("No materials applied to armor record with UID: {}",
                   saveRecord.uid);
      continue;
    }
    ArmorMaterialRecord record;
    for (const auto& material : saveRecord.appliedMaterials) {
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
  ApplyDefaultMaterial(actor, refr->Get3D());
  Helpers::VisitEquippedInventoryItems(
      actor, [&](std::unique_ptr<Helpers::InventoryItem>& item) {
        g_armorMaterials.erase(item->uid);
        ClearItemDisplayName(item->data);
      });
  g_updateStack.push(refr->GetHandle());
}