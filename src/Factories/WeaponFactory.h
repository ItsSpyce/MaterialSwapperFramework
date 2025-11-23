#pragma once

#include "Save/Save.h"

namespace Factories {
class WeaponFactory : public Singleton<WeaponFactory>, public ISaveable {
 public:
  bool ApplyMaterial(RE::Actor* actor, bool leftHand,
                     const MaterialConfig& material);
  bool ApplySavedMaterial(RE::Actor* actor, bool leftHand);
  void ReadFromSave(SKSE::SerializationInterface* iface,
                    Save::SaveData& saveData) override;
  void WriteToSave(SKSE::SerializationInterface* iface,
                   Save::SaveData& saveData) override;

private:
  struct WeaponData {
    vector<string> materials; // copy ArmorFactory?
  };

  emhash8::HashMap<Save::Types::UniqueID, WeaponData> weaponData_; 
};
}  // namespace Factories