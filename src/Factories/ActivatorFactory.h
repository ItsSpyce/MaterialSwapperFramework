#pragma once

namespace Factories {
class ActivatorFactory : public Singleton<ActivatorFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectACTI* form,
                     const MaterialConfig& material) {
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESObjectACTI*) {
    return false;
  }
};
}  // namespace Factories