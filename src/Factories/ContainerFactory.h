#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class ContainerFactory : public Factory<RE::TESObjectCONT>,
                         public Singleton<ContainerFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectCONT* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to container forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectCONT*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for containers can be added here
    return false;
  }
};
}  // namespace Factories