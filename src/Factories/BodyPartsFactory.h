#pragma once

namespace Factories {
class BodyPartsFactory : public Singleton<BodyPartsFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSBodyPart* form,
                     const MaterialConfig& material) {
    // Body parts do not support materials
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSBodyPart*) {
    // Body parts do not support materials
    return false;
  }
};
}  // namespace Factories