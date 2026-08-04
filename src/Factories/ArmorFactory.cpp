#include "ArmorFactory.h"

#include <srell.hpp>

#include "Core/MaterialScope.h"
#include "Graphics/MaterialManager.h"
#include "Helpers/MaterialHelpers.h"
#include "Helpers/RaceMenuHelpers.h"
#include "Helpers/SkyrimHelpers.h"
#include "IO/MaterialLoader.h"
#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"
#include "Save/Types.h"

using ArmorFactory = Factories::ArmorFactory;
using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using Texture = RE::BSShaderTextureSet::Texture;
using VisitControl = RE::BSVisit::BSVisitControl;

bool ArmorFactory::ApplyMaterial(RE::Actor* actor, RE::InventoryEntryData* data,
                                 const MaterialConfig* material,
                                 bool overwriteName) {
  if UNLIKELY(!data) return false;
  auto* armo = data->object->As<RE::TESObjectARMO>();
  if UNLIKELY(!armo) return false;
  auto uid = Helpers::GetUniqueID(actor, *armo->GetSlotMask(), true);
  if (uid == NULL) {
    _WARN("Failed to get unique ID for form: {}", armo->GetFormID());
    return false;
  }
  if UNLIKELY(!MaterialManager::ApplyMaterialToRefr(actor, material)) {
    _ERROR("Failed to apply material to reference: {}, form: {}, unique ID: {}",
           actor->GetFormID(), armo->GetFormID(), uid);
    return false;
  }
  auto [it, _] = armorData_.try_emplace(uid, ArmorData{});
  it->second.actorRefID =
      actor->GetFormID();  // keep the ref ID updated no matter what
  if (std::ranges::contains(it->second.materials, material->name)) {
    return true;  // Material already applied
  }
  vector<string> newAppliedMaterials;
  for (const auto& mat : it->second.materials) {
    const auto* matConfig =
        MaterialLoader::GetMaterialConfig(armo->GetFormID(), mat);
    if (matConfig && matConfig->layer != material->layer) {
      newAppliedMaterials.push_back(mat);
    }
  }

  newAppliedMaterials.push_back(material->name);
  it->second.materials = newAppliedMaterials;
  if (overwriteName) {
    vector<const char*> filteredMaterials;
    VisitAppliedMaterials(data->object->GetFormID(), uid,
                          [&](const char* name, const MaterialConfig& config) {
                            if (!config.isHidden && config.modifyName) {
                              filteredMaterials.emplace_back(name);
                            }
                            return VisitControl::kContinue;
                          });

    if (filteredMaterials.empty()) {
      Helpers::ResetDisplayName(data);
    } else {
      string name = data->object->GetName();
      for (auto& filteredMaterial : filteredMaterials) {
        name += fmt::format("{}, ", filteredMaterial);
      }
      Helpers::SetItemDisplayName(data, name.c_str());
    }
  }

  return true;
}

RE::NiAVObject* ArmorFactory::ApplySavedMaterials(RE::Actor* actor, RE::NiNode* armor,
                                       RE::NiAVObject*, i32 bipedSlot) {
  if UNLIKELY(!actor) return nullptr;
  auto* armorInSlot = actor->GetWornArmor(
      static_cast<RE::BGSBipedObjectForm::BipedObjectSlot>(1 << bipedSlot));
  auto uid = armorInSlot ? Helpers::GetUniqueID(
                               actor, *armorInSlot->GetSlotMask(), false)
                         : 0;
  const auto armorData = uid != 0 ? armorData_.try_get(uid) : nullptr;
  RE::BSVisit::TraverseScenegraphObjects(
      armor, [&](RE::NiAVObject* geometry) {
        auto* triShape = geometry->AsTriShape();
        if (!triShape) {
          return VisitControl::kContinue;
        }
        const auto& material =
            triShape->GetGeometryRuntimeData().shaderProperty;

        if (armorData && armorInSlot) {
          for (const auto& materialName : armorData->materials) {
            auto* materialConfig = MaterialLoader::GetMaterialConfig(
                armorInSlot->GetFormID(), materialName);
            if (!materialConfig) {
              _ERROR("No material config found for material: {}", materialName);
              continue;
            }
            auto appliesEntry =
                materialConfig->applies.contains(triShape->name.c_str())
                    ? materialConfig->applies.at(triShape->name.c_str())
                    : "";
            if UNLIKELY(appliesEntry.empty()) {
              continue;
            }
            const auto* materialFile =
                MaterialLoader::LoadMaterial(appliesEntry);
            if (!materialFile) {
              _ERROR("Failed to load material file: {}", appliesEntry);
              continue;
            }
            MaterialScope scope(materialConfig, materialFile);
            _TRACE("Applying saved material: {}", materialName);
            MaterialManager::ApplyMaterialToNode(
                triShape, materialFile,
                MaterialHelpers::GetMaterialShapeKey(
                    actor->GetFormID(), appliesEntry, materialName));
          }
        } else if (material && std::string(material->name).ends_with(".json")) {
          const auto* materialFile =
              MaterialLoader::LoadMaterial(material->name.c_str());
          if (!materialFile) {
            _ERROR("Failed to load material file: {}", material->name.c_str());
            return VisitControl::kContinue;
          }
          MaterialScope scope(materialFile);
          MaterialManager::ApplyMaterialToNode(
              triShape, materialFile,
              MaterialHelpers::GetMaterialShapeKey(actor->GetFormID(),
                                                   triShape->name.c_str(),
                                                   material->name.c_str()));
        }
        return VisitControl::kContinue;
      });

  return armor;
}

void ArmorFactory::ReadFromSave(Save::SaveData& saveData) {
  for (const auto& saveRecords : saveData.armorRecords | views::values) {
    for (auto& [uid, appliedMaterials] : saveRecords) {
      if (appliedMaterials.empty()) {
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
      armorData_.emplace(uid,
                         ArmorData{.materials = std::move(validMaterials)});
    }
  }
}

void ArmorFactory::WriteToSave(Save::SaveData& saveData) {
  saveData.armorRecords.clear();
  for (auto& [uniqueID, records] : armorData_) {
    if (records.materials.empty()) {
      continue;
    }
    const auto formID = Helpers::GetFormIDForUniqueID(uniqueID);
    if (formID == 0) {
      _WARN("Invalid form ID for unique ID: {}", uniqueID);
      continue;
    }
    Save::Types::ArmorRecordEntryV2 record{
        .uniqueID = uniqueID, .appliedMaterials = records.materials};
    if (saveData.armorRecords.contains(formID)) {
      saveData.armorRecords[formID].push_back(record);
    } else {
      saveData.armorRecords.emplace(formID, vector{record});
    }
  }
}

void ArmorFactory::ResetMaterials(RE::Actor* actor) {
  if UNLIKELY(!actor) {
    return;
  }
  // TODO: fix this
  Helpers::VisitInventoryItems(actor, [&](const Helpers::InventoryItem* item) {
    ResetMaterial(actor, item->data.get());
    return VisitControl::kContinue;
  });
}

void ArmorFactory::ResetMaterial(RE::Actor* actor,
                                 const RE::InventoryEntryData* data) {
  if UNLIKELY(!actor) {
    return;
  }
  auto uid = Helpers::GetUniqueID(actor, data, false);
  if (!uid) {
    return;
  }
  auto toErase = armorData_.find(uid);
  armorData_.erase(toErase);
  Helpers::ResetDisplayName(data);
}

void ArmorFactory::VisitAppliedMaterials(
    const RE::FormID formID, const Save::Types::UniqueID uid,
    const Visitor<const char*, const MaterialConfig&>& visitor) const {
  if (uid == NULL) {
    return;
  }
  auto* armo = RE::TESForm::LookupByID<RE::TESObjectARMO>(formID);
  if (!armo) {
    _ERROR("Form ID {} is not a valid armor", formID);
    return;
  }

  const auto appliedMaterials = armorData_.try_get(uid);
  if (!appliedMaterials) {
    return;
  }
  for (const auto& materialName : appliedMaterials->materials) {
    if (auto* materialConfig =
            MaterialLoader::GetMaterialConfig(formID, materialName)) {
      if (visitor(materialName.c_str(), *materialConfig) ==
          VisitControl::kStop) {
        return;
      }
    }
  }
}

void ArmorFactory::VisitAppliedMaterials(
    RE::Actor* actor,
    const Visitor<RE::TESObjectARMO*, const char*, MaterialConfig&>& visitor)
    const {
  for (const auto& [uid, data] : armorData_) {
    if (data.actorRefID == actor->GetFormID()) {
      auto* form = Helpers::GetFormForUniqueID(uid);
      if (auto* armo = form ? form->As<RE::TESObjectARMO>() : nullptr) {
        for (const auto& material : data.materials) {
          auto* config =
              MaterialLoader::GetMaterialConfig(armo->GetFormID(), material);
          if (config) {
            if (visitor(armo, material.c_str(), *config) ==
                RE::BSVisit::BSVisitControl::kStop) {
              return;
            }
          }
        }
      }
    }
  }
}