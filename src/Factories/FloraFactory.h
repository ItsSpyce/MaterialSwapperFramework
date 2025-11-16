#pragma once

namespace Factories {
class FloraFactory : public Singleton<FloraFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESFlora* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to flora forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESFlora*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for flora can be added here
    return false;
  }
};
}  // namespace Factories