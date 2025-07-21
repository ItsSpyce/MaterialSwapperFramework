#pragma once

#include "Offset.h"

// from https://github.com/SeaSparrowOG/DropBehaviorModifier/blob/master/src/RE/Misc.h

namespace RE {
inline int Demand(const char* a_modelPath, NiPointer<NiNode>& a_modelOut, const BSModelDB::DBTraits::ArgsType& a_args) {
  using func_t = decltype(&Demand);
  static REL::Relocation<func_t> func{Offset::NiAVObject::Demand};
  return func(a_modelPath, a_modelOut, a_args);
}

static void GetTexture(const char* a_path, bool a_demand, NiPointer<NiTexture>& a_out, bool a_4) {
  using func_t = decltype(&GetTexture);
  static REL::Relocation<func_t> func{Offset::NiTexture::GetTexture};
  return func(a_path, a_demand, a_out, a_4);
}

static NiAVObject* CloneNiAVObject(NiAVObject* a_original) {
  using func_t = decltype(&CloneNiAVObject);
  static REL::Relocation<func_t> func{Offset::NiAVObject::Clone};
  return func(a_original);
}
}