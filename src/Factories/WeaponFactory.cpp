#include "WeaponFactory.h"

namespace Factories {
bool WeaponFactory::ApplyMaterial(RE::TESObjectREFR* refr,
                                  RE::TESObjectWEAP* form,
                                  const MaterialConfig& material) {
  // Implementation for applying material to weapon
  return true;
}

bool WeaponFactory::ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                                         RE::TESObjectWEAP*) {
  // Implementation for applying default material to weapon
  return true;
}
}  // namespace Factories