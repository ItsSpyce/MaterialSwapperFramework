#pragma once

#include "Factory.h"
#include "Singleton.h"

namespace Factories {
class IngestibleFactory : public Factory<RE::AlchemyItem>,
                          public Singleton<IngestibleFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::AlchemyItem* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    // Logic to apply material to ingestible forms can be added here
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::AlchemyItem*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for ingestibles can be added here
    return false;
  }
};
}  // namespace Factories