#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class FurnitureFactory : public Factory<RE::TESFurniture>,
                         public Singleton<FurnitureFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESFurniture* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to furniture forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESFurniture*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for furniture can be added here
    return false;
  }
};
}  // namespace Factories