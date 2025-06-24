#pragma once

namespace Factories {

class ArmorFactory;

template <typename Form>
class Factory {
public:
  virtual ~Factory() = default;

  virtual bool ApplyMaterial(RE::TESObjectREFR* refr, Form* form, const MaterialRecord& material) = 0;
  virtual bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, Form* form) = 0;
  virtual bool ApplySavedMaterial(RE::TESObjectREFR* refr, Form* form) = 0;
};
}