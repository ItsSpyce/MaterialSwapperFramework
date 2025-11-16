#pragma once

namespace Factories {
class SoulGemFactory : public Singleton<SoulGemFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESSoulGem* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to soul gem forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESSoulGem*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for soul gems can be added here
    return false;
  }
};
}  // namespace Factories