#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class FloraFactory : public Factory<RE::TESFlora>,
                     public Singleton<FloraFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESFlora* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to flora forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESFlora*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for flora can be added here
    return false;
  }
};
}  // namespace Factories