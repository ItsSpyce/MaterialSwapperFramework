#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class LandscapeFactory : public Factory<RE::TESObjectLAND>,
                         public Singleton<LandscapeFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectLAND* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to landscape forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectLAND*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for landscapes can be added here
    return false;
  }
};
}  // namespace Factories