#pragma once

namespace Factories {
class IngredientFactory : public Singleton<IngredientFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::IngredientItem* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to ingredient forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::IngredientItem*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for ingredients can be added here
    return false;
  }
};
}  // namespace Factories