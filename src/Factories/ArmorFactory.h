#pragma once

#include "IO/MaterialLoader.h"
#include "Save/Save.h"

namespace Factories {
class ArmorFactory : public Singleton<ArmorFactory> {
 public:
  static void ResetMaterials(RE::TESObjectREFR* refr);

  void OnUpdate();
  bool ApplyMaterial(RE::Actor* refr, RE::TESObjectARMO* form,
                     const std::unique_ptr<MaterialConfig>& material);
  bool ApplySavedMaterials(RE::Actor* refr);
  void LoadFromSave(Save::SaveData& saveData);
  void WriteToSave(Save::SaveData& saveData);
};
}  // namespace Factories