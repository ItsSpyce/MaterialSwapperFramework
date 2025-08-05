#pragma once

#include "Factory.h"


namespace Factories {
class WeaponFactory : public Factory<RE::TESObjectWEAP>,
                      public Singleton<WeaponFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectWEAP* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to weapon forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectWEAP*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for weapons can be added here
    return false;
  }
};
}  // namespace Factories