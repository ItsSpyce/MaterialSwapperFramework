#pragma once

#include "Factory.h"


namespace Factories {
class AmmunitionFactory : public Factory<RE::TESAmmo>,
                          public Singleton<AmmunitionFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESAmmo* form,
                     const MaterialConfig& material) override {
    // Ammunition does not support materials
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESAmmo*) override {
    // Ammunition does not support materials
    return false;
  }
};
}  // namespace Factories