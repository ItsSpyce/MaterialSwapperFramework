#include "WeaponFactory.h"

#include "Graphics/MaterialManager.h"
#include "Helpers/MaterialHelpers.h"
#include "Helpers/SkyrimHelpers.h"
#include "IO/MaterialLoader.h"

namespace Factories {
bool WeaponFactory::ApplyMaterial(RE::Actor* actor, bool leftHand,
                                  const MATC& material) {
  RETURN_IF_FALSE(actor)
  auto* weapon = actor->GetEquippedObject(leftHand)->As<RE::TESObjectWEAP>();
  RETURN_IF_FALSE(weapon)
  auto* weaponData = actor->GetEquippedEntryData(leftHand);
  RETURN_IF_FALSE(weaponData)
  return false;
}

bool WeaponFactory::ApplySavedMaterial(RE::Actor* actor, bool leftHand) {
  RETURN_IF_FALSE(actor);
  if (!actor->Is3DLoaded()) {
    return false;
  }
  auto* weapon = actor->GetEquippedObject(leftHand)->As<RE::TESObjectWEAP>();
  const auto slot = Helpers::GetWeaponSlot(weapon);
  if (slot == RE::BIPED_OBJECT::kNone) {
    return false;
  }
  auto* weaponData = actor->GetEquippedEntryData(leftHand);
  auto uid = Helpers::GetUniqueID(actor, slot, true);
  auto& weaponModel = actor->IsPlayerRef()
                          ? weapon->firstPersonModelObject->model
                          : weapon->model;
  RE::NiPointer<RE::NiNode> weaponNode;
  if (RE::BSModelDB::Demand(weaponModel.c_str(), weaponNode,
                            RE::BSModelDB::DBTraits::ArgsType{}) !=
      RE::BSResource::ErrorCode::kNone) {
    _ERROR("Failed to load weapon model: {}", weaponModel.c_str());
    return false;
  }
  auto* rootNode = actor->Get3D()->GetObjectByName(
      fmt::format("Weapon  ({:08X})", weapon->GetFormID()));
  auto* materialInfo = weaponData_.try_get(uid);
  if (!materialInfo) {
    return false;
  }
  for (const auto& materialName : materialInfo->materials) {
    const auto* materialConfig =
        MaterialLoader::GetMaterialConfig(weapon->GetFormID(), materialName);
    if (!materialInfo) {
      _WARN("Material config not found: {}", materialName);
      continue;
    }
    for (const auto& [shape, materialFile] : materialConfig->applies) {
      auto* childShape = rootNode->GetObjectByName(shape)->AsGeometry();
      if (!childShape) {
        _WARN("Shape not found: {}", shape);
        continue;
      }
      const auto* material = MaterialLoader::LoadMaterial(materialFile);
      if (!material) {
        _WARN("Material file not found: {}", materialFile);
        continue;
      }
      if (!MaterialManager::ApplyMaterialToNode(
              childShape, material,
              MaterialHelpers::GetMaterialShapeKey(actor->GetFormID(),
                                                   childShape->name.c_str(),
                                                   materialName))) {
        _WARN("Failed to apply material: {} to shape: {}", materialFile, shape);
      }
    }
  }
  return true;
}

void WeaponFactory::ReadFromSave(Save::SaveData& saveData) {
  for (const auto& formRecords : saveData.weaponRecords | views::values) {
    for (const auto& [uid, materials] : formRecords) {
      if (!uid) {
        _WARN("UID is empty, skipping");
        continue;
      }
      weaponData_[uid] = {
          .materials = materials,
      };
    }
  }
}

void WeaponFactory::WriteToSave(Save::SaveData& saveData) {
  saveData.weaponRecords.clear();
  for (const auto& [uid, weaponData] : weaponData_) {
    if (weaponData.materials.empty()) {
      continue;
    }
    auto formID = Helpers::GetFormIDForUniqueID(uid);
    if (!formID) {
      _WARN("FormID came back as 0, skipping");
      continue;
    }
    Save::Types::WeaponRecordEntryV2 record{
        .uniqueID = uid,
        .appliedMaterials = weaponData.materials,
    };
    if (saveData.weaponRecords.contains(formID)) {
      saveData.weaponRecords[formID].push_back(record);
    } else {
      saveData.weaponRecords.emplace(formID, vector{record});
    }
  }
}
}  // namespace Factories