#pragma once

namespace Factories {
class ClimateFactory : public Singleton<ClimateFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESClimate* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to climate forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESClimate*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for climates can be added here
    return false;
  }
};
}  // namespace Factories