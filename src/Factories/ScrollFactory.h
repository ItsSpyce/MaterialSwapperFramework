#pragma once

namespace Factories {
class ScrollFactory : public Singleton<ScrollFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::ScrollItem* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to scroll forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::ScrollItem*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for scrolls can be added here
    return false;
  }
};
}  // namespace Factories