#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class ExplosionFactory : public Factory<RE::BGSExplosion>,
                         public Singleton<ExplosionFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSExplosion* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to explosion forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::BGSExplosion*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for explosions can be added here
    return false;
  }
};
}  // namespace Factories