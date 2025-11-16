#pragma once

namespace Factories {
class HeadPartFactory : public Singleton<HeadPartFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSHeadPart* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to head part forms can be added here
    
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSHeadPart*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for head parts can be added here
    return false;
  }
};
}  // namespace Factories