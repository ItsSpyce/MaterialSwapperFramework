#pragma once

#include "Offset.h"

// from
// https://github.com/SeaSparrowOG/DropBehaviorModifier/blob/master/src/RE/Misc.h

namespace RE {
inline int Demand(const char* a_modelPath, NiPointer<NiNode>& a_modelOut,
                  const BSModelDB::DBTraits::ArgsType& a_args) {
  using func_t = decltype(&Demand);
  static REL::Relocation<func_t> func{Offset::NiAVObject::Demand};
  return func(a_modelPath, a_modelOut, a_args);
}

static void GetTexture(const char* a_path, bool a_demand,
                       NiPointer<NiTexture>& a_out, bool a_4) {
  using func_t = decltype(&GetTexture);
  static REL::Relocation<func_t> func{Offset::NiTexture::GetTexture};
  return func(a_path, a_demand, a_out, a_4);
}

static NiTexture* LoadTexture(const BSFixedString& name) {
  using func_t = decltype(&LoadTexture);
  static REL::Relocation<func_t> func{Offset::NiTexture::LoadTexture};
  return func(name);
}

static bool LoadTexture(const string& name, NiPointer<NiSourceTexture>& out) {
  const auto newTexture = NiPointer(static_cast<NiSourceTexture*>(LoadTexture(name)));
  if (!newTexture) {
    return false;
  }
  out = newTexture;
  return true;
}

static NiAVObject* CloneNiAVObject(NiAVObject* a_original) {
  using func_t = decltype(&CloneNiAVObject);
  static REL::Relocation<func_t> func{Offset::NiAVObject::Clone};
  return func(a_original);
}

static void SetShaderTexture(BSTextureSet* a_set,
                             BSShaderTextureSet::Texture a_texture,
                             NiSourceTexture* a_srcTexture) {
  using func_t = decltype(&SetShaderTexture);
  static REL::Relocation<func_t> func{Offset::BSShaderTextureSet::SetTexture};
  return func(a_set, a_texture, a_srcTexture);
}

static void SetShaderTexturePath(BSTextureSet* a_set,
                                 BSShaderTextureSet::Texture a_texture,
                                 const char* a_path) {
  using func_t = decltype(&SetShaderTexturePath);
  static REL::Relocation<func_t> func{
      Offset::BSShaderTextureSet::SetTexturePath};
  return func(a_set, a_texture, a_path);
}
}  // namespace RE