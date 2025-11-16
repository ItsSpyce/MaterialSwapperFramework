#pragma once

#include "Save/Save.h"

struct MaterialConfig;


namespace Factories {
class ArmorFactory : public ISaveable, public Singleton<ArmorFactory> {
 public:
  void ResetMaterials(RE::TESObjectREFR* refr);
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectARMO* form,
                     const MaterialConfig* material, bool overwriteName);
  bool ApplySavedMaterials(RE::Actor* actor, RE::NiNode* armor, RE::NiAVObject* attachedAt, i32 bipedSlot);
  void VisitAppliedMaterials(RE::FormID formID, Save::Types::UniqueID uid,
                             const Visitor<const char*, const MaterialConfig&>& visitor) const;
  void ReadFromSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) override;
  void WriteToSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) override;
  void ClearAllData() { armorData_.clear(); }

 private:
  struct ArmorData {
    vector<string> materials;
  };
  emhash8::HashMap<Save::Types::UniqueID, ArmorData> armorData_;
};
}  // namespace Factories