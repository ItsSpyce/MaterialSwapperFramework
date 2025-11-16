#pragma once

namespace Factories {
class BookFactory : public Singleton<BookFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectBOOK* form,
                     const MaterialConfig& material) {
    return false;
  }
  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, RE::TESObjectBOOK*) {
    RETURN_IF_FALSE(refr)
    // Default material application logic for books can be added here
    return false;
  }
};
}  // namespace Factories