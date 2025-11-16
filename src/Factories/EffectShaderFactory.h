#pragma once

namespace Factories {
class EffectShaderFactory : public Singleton<EffectShaderFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESEffectShader* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to effect shader forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESEffectShader*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for effect shaders can be added here
    return false;
  }
};
}  // namespace Factories