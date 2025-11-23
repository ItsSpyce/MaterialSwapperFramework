#include "WeaponFactory.h"

#include "Helpers.h"
#include "RE/Misc.h"

namespace Factories {
bool WeaponFactory::ApplyMaterial(RE::Actor* actor, bool leftHand,
                                  const MaterialConfig& material) {
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
  RE::NiPointer<RE::NiNode> weaponMesh;
  if (!RE::Demand(weapon->GetModel(), weaponMesh, RE::BSModelDB::DBTraits::ArgsType{})) {
    _ERROR("Failed to load model for weapon {0:X}", weapon->GetFormID());
    return false;
  }
  RE::BSVisit::TraverseScenegraphObjects(weaponMesh.get(), [](RE::NiAVObject* obj) {


    return RE::BSVisit::BSVisitControl::kContinue;
  });
}

void WeaponFactory::ReadFromSave(SKSE::SerializationInterface* iface,
                                 Save::SaveData& saveData) {
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

void WeaponFactory::WriteToSave(SKSE::SerializationInterface* iface,
                                Save::SaveData& saveData) {
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
    WeaponRecordEntryV2 record{
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