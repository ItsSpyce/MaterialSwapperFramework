#pragma once

#include "Factory.h"


namespace Factories {
class SoulGemFactory : public Factory<RE::TESSoulGem>,
                       public Singleton<SoulGemFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESSoulGem* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to soul gem forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESSoulGem*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for soul gems can be added here
    return false;
  }
};
}  // namespace Factories