#pragma once

#include "MaterialLoader.h"

namespace MaterialWriter {


bool ApplyMaterial(RE::TESObjectREFR* refr,
                          RE::BIPED_OBJECTS::BIPED_OBJECT armorSlot,
                          const MaterialRecord& material);

bool ApplyBuiltInMaterials(RE::TESObjectREFR* refr);

bool ApplySavedMaterials(RE::TESObjectREFR* refr);

inline bool ApplySavedMaterials(RE::NiPointer<RE::TESObjectREFR> refr) {
  return ApplySavedMaterials(refr.get());
}
}  // namespace MaterialWriter