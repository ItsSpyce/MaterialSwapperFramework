#pragma once

#include "Save/Save.h"

struct MaterialConfig;

namespace Factories {
class ArmorFactory : public Singleton<ArmorFactory>, public ISaveable {
 public:
  void ResetMaterials(RE::TESObjectREFR* refr);
  void OnUpdate();
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectARMO* form,
                     const MaterialConfig* material, bool overwriteName);
  bool ApplySavedMaterials(RE::TESObjectREFR* refr);
  bool ApplySavedMaterials(RE::TESObjectREFR* refr, RE::TESObjectARMO* armo);
  void VisitAppliedMaterials(Save::Types::UniqueID uid,
                             const Visitor<const char*, const MaterialConfig&>& visitor);
  void ReadFromSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) override;
  void WriteToSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) const override;
  void ClearAllData() { appliedMaterials_.clear(); }

 private:

  struct UpdateRequest {
    RE::TESObjectREFR* refr;
    RE::TESObjectARMO* armo;
  };
  struct AppliedMaterials {
    vector<string> materials;
  };
  unordered_map<Save::Types::UniqueID, AppliedMaterials> appliedMaterials_;
  stack<UpdateRequest> updateStack_;
};
}  // namespace Factories