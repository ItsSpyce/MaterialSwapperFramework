#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class ArtObjectFactory : public Factory<RE::BGSArtObject>,
                         public Singleton<ArtObjectFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::BGSArtObject* form,
                     const MaterialRecord& material) override {
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