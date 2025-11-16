#pragma once

namespace Factories {
class TreeFactory : public Singleton<TreeFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectTREE* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to tree forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESObjectTREE*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for trees can be added here
    return false;
  }
};
}  // namespace Factories