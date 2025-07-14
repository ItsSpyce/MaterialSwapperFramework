#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class TreeFactory : public Factory<RE::TESObjectTREE>,
                    public Singleton<TreeFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectTREE* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to tree forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectTREE*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for trees can be added here
    return false;
  }
};
}  // namespace Factories