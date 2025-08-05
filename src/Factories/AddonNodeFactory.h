#pragma once

#include "Factory.h"

namespace Factories {
class AddonNodeFactory : public Factory<RE::BGSAddonNode>,
                         public Singleton<AddonNodeFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSAddonNode* form,
                     const MaterialConfig& material) override {
    // Implementation for applying material to an AddonNode
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::BGSAddonNode*) override {
    // Implementation for applying default material to an AddonNode
    return false;
  }
};
}  // namespace Factories