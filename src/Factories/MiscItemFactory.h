#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class MiscItemFactory : public Factory<RE::TESObjectMISC>,
                        public Singleton<MiscItemFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectMISC* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to misc item forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectMISC*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for misc items can be added here
    return false;
  }
};
}  // namespace Factories