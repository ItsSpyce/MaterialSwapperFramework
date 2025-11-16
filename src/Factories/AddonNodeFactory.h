#pragma once

namespace Factories {
class AddonNodeFactory : public Singleton<AddonNodeFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSAddonNode* form,
                     const MaterialConfig& material) {
    // Implementation for applying material to an AddonNode
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSAddonNode*) {
    // Implementation for applying default material to an AddonNode
    return false;
  }
};
}  // namespace Factories