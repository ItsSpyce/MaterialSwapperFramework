#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class DoorFactory : public Factory<RE::TESObjectDOOR>,
                    public Singleton<DoorFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectDOOR* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to door forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectDOOR*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for doors can be added here
    return false;
  }
};
}  // namespace Factories