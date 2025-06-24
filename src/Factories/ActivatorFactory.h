#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class ActivatorFactory : public Factory<RE::TESObjectACTI>,
                         public Singleton<ActivatorFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectACTI* form,
                     const MaterialRecord& material) override {
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectACTI*) override {
    return false;
  }
};
}  // namespace Factories