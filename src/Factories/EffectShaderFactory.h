#pragma once

#include "Factory.h"


namespace Factories {
class EffectShaderFactory : public Factory<RE::TESEffectShader>,
                            public Singleton<EffectShaderFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESEffectShader* form,
                     const MaterialConfig& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to effect shader forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESEffectShader*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for effect shaders can be added here
    return false;
  }
};
}  // namespace Factories