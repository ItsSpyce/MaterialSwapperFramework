#pragma once

#include <emhash/hash_table8.hpp>
#include "Save/Save.h"

namespace Factories {
class WeaponFactory : public Singleton<WeaponFactory>, public ISaveable {
 public:
  bool ApplyMaterial(RE::Actor* actor, bool leftHand,
                     const MATC& material);
  bool ApplySavedMaterial(RE::Actor* actor, bool leftHand);
  void ReadFromSave(Save::SaveData& saveData) override;
  void WriteToSave(Save::SaveData& saveData) override;
};
}  // namespace Factories