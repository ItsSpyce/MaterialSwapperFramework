#pragma once

namespace Factories {
class MoveableStaticFactory : public Singleton<MoveableStaticFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSMovableStatic* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to moveable static forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSMovableStatic*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for moveable statics can be added here
    return false;
  }
};
}  // namespace Factories