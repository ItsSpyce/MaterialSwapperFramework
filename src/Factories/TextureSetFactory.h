#pragma once

namespace Factories {
class TextureSetFactory : public Singleton<TextureSetFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSTextureSet* form,
                     const MaterialConfig& material) {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to texture set forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSTextureSet*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for texture sets can be added here
    return false;
  }
};
}  // namespace Factories