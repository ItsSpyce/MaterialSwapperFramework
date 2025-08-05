#pragma once

#include "Factory.h"


namespace Factories {
class ArtObjectFactory : public Factory<RE::BGSArtObject>,
                         public Singleton<ArtObjectFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSArtObject* form,
                     const MaterialConfig& material) override {
    // Art objects do not support materials
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::BGSArtObject*) override {
    // Art objects do not support materials
    return false;
  }
};
}  // namespace Factories