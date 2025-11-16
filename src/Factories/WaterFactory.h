#pragma once

namespace Factories {
class WaterFactory : public Singleton<WaterFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESWaterForm* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to water forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESWaterForm*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for water can be added here
    return false;
  }
};
}  // namespace Factories