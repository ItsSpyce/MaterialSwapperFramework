#pragma once

#include "Factory.h"

namespace Factories {
class BodyPartsFactory : public Factory<RE::BGSBodyPart>,
                         public Singleton<BodyPartsFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSBodyPart* form,
                     const MaterialRecord& material) override {
    // Body parts do not support materials
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::BGSBodyPart*) override {
    // Body parts do not support materials
    return false;
  }
};
}  // namespace Factories