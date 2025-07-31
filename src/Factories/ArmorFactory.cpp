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

static RE::NiAlphaProperty* GetAlphaProperty(RE::BSTriShape* bsTriShape) {
  auto* properties = bsTriShape->properties[2].get();
  auto* alphaProperty =
      properties ? netimmerse_cast<RE::NiAlphaProperty*>(properties) : nullptr;
  return alphaProperty;
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
    logger::error("Failed to cast texture to NiSourceTexture for map: {}",
                  path);
    return nullptr;
  }
  return newTexture;
}

static void ApplyTexture(RE::NiPointer<RE::NiSourceTexture>& target, const char* path) {
  auto* texture = LoadTexture(path);
  
}

static bool ApplyMaterialToNode(RE::TESObjectREFR* refr, bool,
                                RE::BSTriShape* bsTriShape,
                                const std::unique_ptr<MaterialRecord>& record) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(bsTriShape)
  auto* shaderProperty = GetShaderProperty(bsTriShape);
  auto* base =
      skyrim_cast<RE::BSLightingShaderMaterialBase*>(shaderProperty->material);
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
  Helpers::VisitEquippedInventoryItems(
      refr.get(), [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
        if (!ApplySavedMaterial_Impl(this, refr.get()->As<RE::Actor>(), item)) {
          logger::error("Failed to apply saved material for reference: {}",
                        refr->GetFormID());
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

bool ArmorFactory::ApplySavedMaterials(RE::Actor* refr) {
  RETURN_IF_FALSE(refr)
  g_updateStack.push(refr->GetHandle());
  return true;
}

bool ArmorFactory::ApplySavedMaterialsImmediate(RE::Actor* refr,
                                                RE::NiAVObject* node) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(node)
  Helpers::VisitEquippedInventoryItems(
      refr, [&](const std::unique_ptr<Helpers::InventoryItem>& item) {
        if (item->uid == NULL) {
          ApplyDefaultMaterial(refr, node);
          return;
        }
        auto& [appliedMaterials] = g_armorMaterials[item->uid];
        logger::debug("Applying materials: {}",
                      fmt::join(appliedMaterials, ", "));
        for (const auto& materialName : appliedMaterials) {
          auto appliedMaterialConfig = MaterialLoader::GetMaterialConfig(
              item->data->object->GetFormID(), materialName);
          if (!appliedMaterialConfig) {
            ApplyDefaultMaterial(refr, node);
            logger::warn(
                "No material config found for form ID: {}, material: {}",
                item->data->object->GetFormID(), materialName);
            continue;
          }
          for (const auto& [shapeName, materialFile] :
               appliedMaterialConfig->applies) {
            auto* nivAv = node->GetObjectByName(shapeName);
            if (!nivAv) {
              logger::warn("No object found for shape name: {}", shapeName);
              continue;
            }
            auto* triShape = nivAv->AsTriShape();
            if (!triShape) {
              logger::warn("No tri-shape found for shape name: {}", shapeName);
              continue;
            }
            if (!ApplyMaterialToNode(refr, true, triShape,
                                     MaterialLoader::LoadMaterial(materialFile))) {
              logger::error(
                  "Failed to apply material to tri-shape: {}, material: {}",
                  triShape->name, materialFile);
            }
          }
        }
      });
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
  // TODO:
}