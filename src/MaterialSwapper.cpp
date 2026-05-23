#include "MaterialSwapper.h"

#include "Core/MaterialScope.h"
#include "Graphics/MaterialManager.h"
#include "Helpers/MaterialHelpers.h"
#include "Helpers/RaceMenuHelpers.h"
#include "IO/MaterialLoader.h"
#include "Models/MaterialConfig.h"

namespace {
struct StoredMaterial {
  RE::FormID refID;
  vector<string> materials;
};

emhash8::HashMap<Save::Types::UniqueID, StoredMaterial> g_data;

RE::NiNode* CloneWithMaterial(RE::NiNode* node, RE::FormID formId,
                              const vector<string>& materials) {
  auto* clone = (RE::NiNode*)node->Clone();
  RE::BSVisit::TraverseScenegraphObjects(clone, [&](RE::NiAVObject* geometry) {
    auto* ts = geometry->AsTriShape();
    if (!ts) return RE::BSVisit::BSVisitControl::kContinue;
    const auto& material = ts->GetGeometryRuntimeData().shaderProperty;
    if (material && material->name.contains(".json")) {
      const auto* materialRecord =
          MaterialLoader::LoadMaterial(material->name.c_str());
      if (materialRecord) {
        // no caching for those with built-in material points yet
        MaterialManager::ApplyMaterialToNode(ts, materialRecord, "");
      } else {
        _ERROR("Failed to load material record {}", material->name.c_str());
      }
    }
    for (const auto& materialName : materials) {
      auto* materialConfig =
          MaterialLoader::GetMaterialConfig(formId, materialName);
      if (!materialConfig) {
        _ERROR("No material config found for material {}", materialName);
        continue;
      }
      if (auto it = materialConfig->applies.find(ts->name.c_str());
          it != materialConfig->applies.end()) {
        const auto* materialRecord = MaterialLoader::LoadMaterial(it->second);
        if (!materialRecord) {
          _ERROR("Failed to load material record {}", it->second);
          continue;
        }
        MaterialScope scope(materialConfig, materialRecord);
        _TRACE("Applying saved material {}", materialName);
        MaterialManager::ApplyMaterialToNode(
            ts, materialRecord,
            MaterialHelpers::GetMaterialShapeKey(formId, it->second,
                                                 materialName));
      }
    }
    return RE::BSVisit::BSVisitControl::kContinue;
  });
  return clone;
}

RE::NiNode* LoadArmorMaterials(
    RE::TESObjectREFR* refr, const MaterialSwapper::LoadArmorRequest& request) {
  auto* actor = refr->As<RE::Actor>();
  if (!actor) return nullptr;
  const auto* armorInSlot = actor->GetWornArmor(
      static_cast<RE::BGSBipedObjectForm::BipedObjectSlot>(1 << request.slot));
  if (!armorInSlot) return nullptr;
  const auto uid =
      Helpers::GetUniqueID(refr, *armorInSlot->GetSlotMask(), false);
  if (const auto it = g_data.find(uid); it != g_data.end()) {
    return CloneWithMaterial(request.armorNode, armorInSlot->GetFormID(),
                             it->second.materials);
  }
  return nullptr;
}

RE::NiNode* UpdateArmorMaterials(
    RE::TESObjectREFR* refr, const MaterialSwapper::UpdateArmorRequest& request,
    const MaterialConfig& config) {
  auto* actor = refr->As<RE::Actor>();
  if (!actor) return nullptr;
  const auto* armorInSlot = actor->GetWornArmor(
      static_cast<RE::BGSBipedObjectForm::BipedObjectSlot>(1 << request.slot));
  if (!armorInSlot) return nullptr;
  const auto uid =
      Helpers::GetUniqueID(refr, *armorInSlot->GetSlotMask(), true);
  auto [it, _] = g_data.try_emplace(uid, StoredMaterial{
                                             .refID = refr->GetFormID(),
                                             .materials = {},
                                         });
  it->second.materials.push_back(config.name);
  if (config.modifyName) {
    vector<string> filteredMaterials;
    // TODO: update name
  }
  return CloneWithMaterial(request.armorNode, armorInSlot->GetFormID(),
                           it->second.materials);
}
}  // namespace

void MaterialSwapper::ReadFromSave(const Save::SaveData& saveData) {
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
      g_data.emplace(uid,
                     StoredMaterial{.materials = std::move(validMaterials)});
    }
  }
}

void MaterialSwapper::WriteToSave(Save::SaveData& saveData) {
  saveData.armorRecords.clear();
  for (auto& [uniqueID, records] : g_data) {
    if (records.materials.empty()) {
      continue;
    }
    const auto formID = Helpers::GetFormIDForUniqueID(uniqueID);
    if (!formID) {
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

template <typename T>
RE::NiNode* MaterialSwapper::LoadMaterials(RE::TESObjectREFR* refr,
                                           const T& request) {
  if (!refr) {
    _ERROR("Invalid refr for load");
    return nullptr;
  }
  static_assert(T::TYPE);
  if constexpr (T::TYPE == kLoadArmorRequest) {
    return LoadArmorMaterials(refr, request);
  }
  return nullptr;
}

template <typename T>
RE::NiNode* MaterialSwapper::UpdateMaterial(RE::TESObjectREFR* refr,
                                            const T& request,
                                            const MaterialConfig& config) {
  // this returns a NiNode to insert into the refr's 3D
  if (!refr) {
    _ERROR("Invalid refr for update");
    return nullptr;
  }
  static_assert(T::TYPE);
  if constexpr (T::TYPE == kUpdateArmorRequest) {
    return UpdateArmorMaterials(refr, request);
  }
  return nullptr;
}
