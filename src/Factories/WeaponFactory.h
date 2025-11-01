#pragma once

#include "Factory.h"
#include "Save/Save.h"


namespace Factories {
class WeaponFactory : public Factory<RE::TESObjectWEAP>,
                      public Singleton<WeaponFactory>,
                      public ISaveable {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectWEAP* form,
                     const MaterialConfig& material) override;
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectWEAP*) override;
  void ReadFromSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) override;
  void WriteToSave(SKSE::SerializationInterface* iface,
                   Save::SaveData& saveData) override;
};
}  // namespace Factories