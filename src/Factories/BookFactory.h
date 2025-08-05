#pragma once

#include "Factory.h"


namespace Factories {
class BookFactory : public Factory<RE::TESObjectBOOK>,
                    public Singleton<BookFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectBOOK* form,
                     const MaterialConfig& material) override {
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectBOOK*) override {
    RETURN_IF_FALSE(refr)
    // Default material application logic for books can be added here
    return false;
  }
};
}  // namespace Factories