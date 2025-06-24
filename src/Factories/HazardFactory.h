#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class HazardFactory : public Factory<RE::BGSHazard>,
                      public Singleton<HazardFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSHazard* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to hazard forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSHazard*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for hazards can be added here
    return false;
  }
};
}  // namespace Factories