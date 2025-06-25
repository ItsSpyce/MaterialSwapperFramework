#pragma once

#include "NifHelpers.h"
#include "Papyrus.h"
#include "Models/MaterialFileBase.h"

namespace NiOverride {
enum : uint32_t {
  kNiOverrideKey_ShaderEmissiveColor = 0,
  kNiOverrideKey_ShaderEmissiveMultiple = 1,
  kNiOverrideKey_ShaderGlossiness = 2,
  kNiOverrideKey_ShaderSpecularStrength = 3,
  kNiOverrideKey_ShaderLightingEffect1 = 4,
  kNiOverrideKey_ShaderLightingEffect2 = 5,
  kNiOverrideKey_ShaderTextureSet = 6,
  kNiOverrideKey_ShaderTintColor = 7,
  kNiOverrideKey_ShaderAlpha = 8,
  kNiOverrideKey_ShaderTexture = 9,
  kNiOverrideKey_ControllerStartStop = 20,
  kNiOverrideKey_ControllerStartTime = 21,
  kNiOverrideKey_ControllerStopTime = 22,
  kNiOverrideKey_ControllerFrequency = 23,
  kNiOverrideKey_ControllerPhase = 24,
};

#define MAKE_NI_OVERRIDE_FUNCTION_ARGS(...) RE::StaticFunctionTag, __VA_ARGS__
#define MAKE_NI_OVERRIDE_FUNCTION(_NAME, _RETURN_TYPE, ...)                \
  typedef _RETURN_TYPE (*_NAME##Fn)(                                       \
      MAKE_NI_OVERRIDE_FUNCTION_ARGS(__VA_ARGS__));                        \
  static inline _NAME##Fn _NAME() {                                        \
    static auto fn = *GetNativeFunction<_NAME##Fn*>("NiOverride", #_NAME); \
    return fn;                                                             \
  }

MAKE_NI_OVERRIDE_FUNCTION(AddOverrideString, void, RE::TESObjectREFR* refr,
                          bool isFemale, RE::TESObjectARMO* armo,
                          RE::TESObjectARMA* arma, const char* node,
                          uint32_t key, uint32_t index, const char* value,
                          bool persist)

MAKE_NI_OVERRIDE_FUNCTION(AddNodeOverrideFloat, void, RE::TESObjectREFR* refr,
                          bool isFemale, const char* node, int key, int index,
                          float value, bool persist)

MAKE_NI_OVERRIDE_FUNCTION(AddNodeOverrideInt, void, RE::TESObjectREFR* refr,
                          bool isFemale, const char* node, int key, int index,
                          int value, bool persist)

MAKE_NI_OVERRIDE_FUNCTION(AddNodeOverrideBool, void, RE::TESObjectREFR* refr,
                          bool isFemale, const char* node, int key, int index,
                          bool value, bool persist)

MAKE_NI_OVERRIDE_FUNCTION(AddNodeOverrideString, void, RE::TESObjectREFR* refr,
                          bool isFemale, const char* node, int key, int index,
                          const char* value, bool persist)

MAKE_NI_OVERRIDE_FUNCTION(AddNodeOverrideTextureSet, void,
                          RE::TESObjectREFR* refr, bool isFemale,
                          const char* node, int key, int index,
                          RE::BSTextureSet* textureSet, bool persist)

MAKE_NI_OVERRIDE_FUNCTION(AddSkinOverrideString, void, RE::TESObjectREFR* refr,
                          bool isFemale, bool firstPerson, int slotMask,
                          uint32_t key, uint32_t index, const char* value,
                          bool persist)

MAKE_NI_OVERRIDE_FUNCTION(GetOverrideString, RE::BSFixedString,
                          RE::TESObjectREFR* refr, bool isFemale,
                          RE::TESObjectARMO* armo, RE::TESObjectARMA* arma,
                          const char* node, int key, uint32_t index)

MAKE_NI_OVERRIDE_FUNCTION(ApplyOverrides, void, RE::TESObjectREFR* refr)

MAKE_NI_OVERRIDE_FUNCTION(RemoveAllReferenceOverrides, void,
                          RE::TESObjectREFR* refr)

MAKE_NI_OVERRIDE_FUNCTION(HasNodeOverride, bool, RE::TESObjectREFR* refr,
                          bool isFemale, const char* node, int key,
                          uint32_t index)

static bool ApplyMaterialToNode(RE::TESObjectREFR* refr, bool isFemale,
                                const char* node,
                                MaterialFileBase& material) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(node)

  AddNodeOverrideFloat()(RE::StaticFunctionTag{}, refr, isFemale, node,
                         kNiOverrideKey_ShaderAlpha, 0, material.transparency,
                         false);

  if (const auto lightingMaterial = material.As<BGSMFile>()) {
    auto diffuseTex = StringHelpers::PrefixTexturesPath(lightingMaterial->diffuseMap);
    auto normalTex =
        lightingMaterial->normalMap.empty()
            ? ""
            : StringHelpers::PrefixTexturesPath(lightingMaterial->normalMap);
    auto specularTex =
        lightingMaterial->specularMap.empty()
            ? ""
            : StringHelpers::PrefixTexturesPath(lightingMaterial->specularMap);
    logger::debug(
        "Applying lighting material to node: {}, diffuse: {}, "
        "normal: {}, specular: {}",
        node, diffuseTex, normalTex, specularTex);

    AddNodeOverrideString()(RE::StaticFunctionTag{}, refr, isFemale, node,
                            kNiOverrideKey_ShaderTexture, 0, diffuseTex.c_str(),
                            false);
    AddNodeOverrideString()(RE::StaticFunctionTag{}, refr, isFemale, node,
                            kNiOverrideKey_ShaderTexture, 1,
                            normalTex.c_str(), false);
    AddNodeOverrideString()(RE::StaticFunctionTag{}, refr, isFemale, node,
                            kNiOverrideKey_ShaderTexture, 7,
                            specularTex.c_str(), false);
    AddNodeOverrideFloat()(RE::StaticFunctionTag{}, refr, isFemale, node,
                           kNiOverrideKey_ShaderSpecularStrength, 1,
                           lightingMaterial->specularMult, false);
    return true;
  }
  if (const auto effectMaterial = material.As<BGEMFile>()) {
    logger::error("BGEM not supported yet");
    return false;
  }
  return false;
}
};  // namespace NiOverride