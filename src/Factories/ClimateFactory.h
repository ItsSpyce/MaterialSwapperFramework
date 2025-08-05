#pragma once

#include "Factory.h"


namespace Factories {
class ClimateFactory : public Factory<RE::TESClimate>,
                       public Singleton<ClimateFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESClimate* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to climate forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESClimate*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for climates can be added here
    return false;
  }
};
}  // namespace Factories