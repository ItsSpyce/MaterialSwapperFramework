#pragma once

namespace Factories {
class StaticFactory : public Singleton<StaticFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectSTAT* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to static forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESObjectSTAT*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for statics can be added here
    return false;
  }
};
}  // namespace Factories