#pragma once

namespace Factories {
class ArtObjectFactory : public Singleton<ArtObjectFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSArtObject* form,
                     const MaterialConfig& material) {
    // Art objects do not support materials
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::BGSArtObject*) {
    // Art objects do not support materials
    return false;
  }
};
}  // namespace Factories