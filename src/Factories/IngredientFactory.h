#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class IngredientFactory : public Factory<RE::IngredientItem>,
                          public Singleton<IngredientFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::IngredientItem* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to ingredient forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::IngredientItem*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for ingredients can be added here
    return false;
  }
};
}  // namespace Factories