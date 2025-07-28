#pragma once

#include "MaterialConfig.h"

namespace Factories {

class ArmorFactory;

template <typename Form>
class Factory {
public:
  virtual ~Factory() = default;

  virtual bool ApplyMaterial(RE::TESObjectREFR* refr, Form* form, const MaterialConfig& material) = 0;
  virtual bool ApplyDefaultMaterial(RE::TESObjectREFR* refr, Form* form) = 0;
  virtual bool ApplySavedMaterial(RE::TESObjectREFR* refr, Form* form) = 0;
};
}