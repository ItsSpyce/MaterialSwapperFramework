#pragma once

#include "Factory.h"


namespace Factories {
class StaticFactory : public Factory<RE::TESObjectSTAT>,
                      public Singleton<StaticFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectSTAT* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to static forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectSTAT*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for statics can be added here
    return false;
  }
};
}  // namespace Factories