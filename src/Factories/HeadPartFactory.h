#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class HeadPartFactory : public Factory<RE::BGSHeadPart>,
                        public Singleton<HeadPartFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSHeadPart* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to head part forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::BGSHeadPart*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for head parts can be added here
    return false;
  }
};
}  // namespace Factories