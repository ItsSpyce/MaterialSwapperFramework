#pragma once

#include "Factory.h"


namespace Factories {
class LightFactory : public Factory<RE::TESObjectLIGH>,
                     public Singleton<LightFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectLIGH* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to light forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectLIGH*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for lights can be added here
    return false;
  }
};
}  // namespace Factories