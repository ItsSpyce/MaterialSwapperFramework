#pragma once

namespace Factories {
class AmmunitionFactory : public Singleton<AmmunitionFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESAmmo* form,
                     const MaterialConfig& material) {
    // Ammunition does not support materials
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESAmmo*) {
    // Ammunition does not support materials
    return false;
  }
};
}  // namespace Factories