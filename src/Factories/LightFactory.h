#pragma once

namespace Factories {
class LightFactory : public Singleton<LightFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectLIGH* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to light forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESObjectLIGH*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for lights can be added here
    return false;
  }
};
}  // namespace Factories