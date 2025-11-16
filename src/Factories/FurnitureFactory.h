#pragma once

namespace Factories {
class FurnitureFactory : public Singleton<FurnitureFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESFurniture* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to furniture forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESFurniture*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for furniture can be added here
    return false;
  }
};
}  // namespace Factories