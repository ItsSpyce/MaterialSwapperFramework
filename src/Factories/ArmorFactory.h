#pragma once

#include "IO/MaterialLoader.h"
#include "Save/Save.h"

namespace Factories {
class ArmorFactory : public Singleton<ArmorFactory>, public ISaveable {
 public:
  void ResetMaterials(RE::TESObjectREFR* refr);
  void OnUpdate();
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectARMO* form,
                     const MaterialConfig* material);
  bool ApplySavedMaterials(RE::TESObjectREFR* refr);
  bool ApplySavedMaterials(RE::TESObjectREFR* refr, RE::TESObjectARMO* armo);
  void VisitAppliedMaterials(
      int uid, const Visitor<const char*, const MaterialConfig&>& visitor);
  void ReadFromSave(Save::SaveData& saveData) override;
  void WriteToSave(Save::SaveData& saveData) const override;

 private:
  struct ArmorMaterialRecord {
    vector<string> appliedMaterials;
  };

  struct UpdateRequest {
    RE::TESObjectREFR* refr;
    RE::TESObjectARMO* armo;
  };
  unordered_map<u32, ArmorMaterialRecord> knownArmorMaterials_;
  stack<UpdateRequest> updateStack_;
};
}  // namespace Factories