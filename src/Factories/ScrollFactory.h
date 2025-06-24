#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class ScrollFactory : public Factory<RE::ScrollItem>,
                      public Singleton<ScrollFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::ScrollItem* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to scroll forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::ScrollItem*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for scrolls can be added here
    return false;
  }
};
}