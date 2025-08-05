#pragma once

#include "Factory.h"


namespace Factories {
class ProjectileFactory : public Factory<RE::BGSProjectile>,
                          public Singleton<ProjectileFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSProjectile* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to projectile forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::BGSProjectile*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for projectiles can be added here
    return false;
  }
};
}  // namespace Factories