#pragma once

#include "Save/Save.h"

struct MaterialConfig;

namespace Factories {
class ArmorFactory : public Singleton<ArmorFactory> {
 public:
  void ResetMaterials(RE::Actor* actor);
  void ResetMaterial(RE::Actor* actor, const RE::InventoryEntryData* data);
  bool ApplyMaterial(RE::Actor* actor, RE::InventoryEntryData* data,
                     const MaterialConfig* material, bool overwriteName);
  RE::NiAVObject* ApplySavedMaterials(RE::Actor* actor, RE::NiNode* armor,
                           RE::NiAVObject* attachedAt, i32 bipedSlot);
  void VisitAppliedMaterials(
      RE::FormID formID, UniqueID uid,
      const Visitor<const char*, const MaterialConfig&>& visitor) const;
  void VisitAppliedMaterials(
      RE::Actor* actor,
      const Visitor<RE::TESObjectARMO*, const char*, MaterialConfig&>&
          visitor) const;
};
}  // namespace Factories