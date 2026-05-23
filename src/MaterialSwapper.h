#pragma once
#include "Save/Save.h"

struct MaterialConfig;

namespace MaterialSwapper {
enum RequestType : uint8_t {
  kUpdateArmorRequest,
  kLoadArmorRequest,
  kLoadWeaponRequest
};
struct UpdateArmorRequest {
  static constexpr RequestType TYPE = kUpdateArmorRequest;
  RE::NiNode* armorNode;
  i32 slot;
};
struct LoadArmorRequest {
  static constexpr RequestType TYPE = kLoadArmorRequest;
  RE::NiNode* armorNode;
  RE::NiAVObject* attachedAt;
  i32 slot;
};

void ReadFromSave(const Save::SaveData& saveData);
void WriteToSave(Save::SaveData& saveData);
void ResetAll();
void ResetMaterial(RE::TESObjectREFR* refr, void* key);
template <typename T>
RE::NiNode* UpdateMaterial(RE::TESObjectREFR* refr, const T& request, const MaterialConfig& config);
template <typename T>
RE::NiNode* LoadMaterials(RE::TESObjectREFR* refr, const T& request);
void VisitMaterials(RE::TESObjectREFR* refr, const Visitor<const char*, const MaterialConfig&>& visitor);
}