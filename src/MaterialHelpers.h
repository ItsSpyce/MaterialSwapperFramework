#pragma once

#include "Models/MaterialRecord.h"

namespace MaterialHelpers {
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;

inline void ApplyFlags(RE::BSLightingShaderProperty* lightingShader,
                       const MaterialRecord* record) {
#define MAP_FLAG(_MATERIAL_FLAG, _SHADER_FLAG)          \
  if (record->_MATERIAL_FLAG.has_value()) {             \
    lightingShader->SetFlags(ShaderFlag8::_SHADER_FLAG, \
                             record->_MATERIAL_FLAG.value());  \
  }

  MAP_FLAG(decal, kDecal)
  MAP_FLAG(twoSided, kTwoSided)
  MAP_FLAG(envMapEnabled, kEnvMap)
  MAP_FLAG(specularEnabled, kSpecular)
  MAP_FLAG(receiveShadows, kReceiveShadows)
  MAP_FLAG(castShadows, kCastShadows)
  MAP_FLAG(facegen, kFace)
  MAP_FLAG(hair, kHairTint)
  MAP_FLAG(decalNoFade, kNoFade)
  MAP_FLAG(nonOccluder, kNonProjectiveShadows)
  MAP_FLAG(refraction, kRefraction)
  MAP_FLAG(refractionalFalloff, kRefractionFalloff)
  MAP_FLAG(grayscaleToPaletteColor, kGrayscaleToPaletteColor)
  MAP_FLAG(depthWrite, kZBufferWrite)
  MAP_FLAG(depthTest, kZBufferTest)
  MAP_FLAG(subsurfaceLighting, kCharacterLighting)
  MAP_FLAG(envMapEye, kEyeReflect)
  MAP_FLAG(emitEnabled, kOwnEmit)
  MAP_FLAG(pbr, kMenuScreen)
}

inline RE::NiColor* GetColorPtr(const array<float, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return new RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                           colorArr[2] / 255.0f);
  }
  return new RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

inline RE::NiColorA* GetColorAPtr(const array<float, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return new RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                            colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return new RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

inline RE::NiColor GetColor(const array<float, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                       colorArr[2] / 255.0f);
  }
  return RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

inline RE::NiColorA GetColorA(const array<float, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                        colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

inline RE::NiPoint2 GetPoint2(const array<float, 2>& pointArr) {
  return RE::NiPoint2(pointArr[0], pointArr[1]);
}

inline RE::NiPoint3 GetPoint3(const array<float, 3>& pointArr) {
  return RE::NiPoint3(pointArr[0], pointArr[1], pointArr[2]);
}

inline const char* GetStringPtr(const std::optional<std::string>& str) {
  if (str && !str->empty()) {
    return str->c_str();
  }
  return nullptr;
}
}  // namespace MaterialHelpers