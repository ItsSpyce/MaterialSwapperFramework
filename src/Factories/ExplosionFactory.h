#pragma once

namespace Factories {
class ExplosionFactory : public Singleton<ExplosionFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSExplosion* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to explosion forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSExplosion*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for explosions can be added here
    return false;
  }
};
}  // namespace Factories