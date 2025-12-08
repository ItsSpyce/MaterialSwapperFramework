#pragma once

#include "Graphics/ShaderManager.h"
#include "Graphics/TextureLoader.h"
#include "IO/MaterialLoader.h"
#include "Models/MaterialRecord.h"
#include "NifHelpers.h"
#include "CommunityShaders/BSLightingShaderMaterialPBR.h"

namespace MaterialHelpers {
using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;

inline void ApplyFlags(RE::BSLightingShaderProperty* lightingShader,
                       const MaterialRecord* record) {
#define MAP_FLAG(_MATERIAL_FLAG, _SHADER_FLAG)                \
  if (record->_MATERIAL_FLAG.has_value()) {                   \
    lightingShader->SetFlags(ShaderFlag8::_SHADER_FLAG,       \
                             record->_MATERIAL_FLAG.value()); \
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

inline void CopyMembers(RE::BSShaderMaterial* from,
                 RE::BSLightingShaderMaterialBase* to) {
  if (auto* fromBase =
          skyrim_cast<RE::BSLightingShaderMaterialBase*>(from)) {
    to->CopyBaseMembers(fromBase);
  }
  to->CopyMembers(to);
}

inline bool ApplyMaterialToNode(
                                RE::BSGeometry* bsTriShape,
                                const MaterialRecord* record) {
  RETURN_IF_FALSE(bsTriShape)
  auto* lightingShader = NifHelpers::GetShaderProperty(bsTriShape);
  auto* alphaProperty = NifHelpers::GetAlphaProperty(bsTriShape);
  auto* newMaterial = RE::BSLightingShaderMaterialBase::CreateMaterial(
      lightingShader->material->GetFeature());
  if (!newMaterial) {
    _ERROR("Failed to create BSLightingShaderMaterialBase for tri shape: {}",
           bsTriShape->name);
    return false;
  }
  CopyMembers(lightingShader->material, newMaterial);
  ApplyFlags(lightingShader, record);
  auto* textureLoader = Graphics::TextureLoader::GetSingleton();

  if (record->pbr) {
    auto* pbrMaterial = (BSLightingShaderMaterialPBR*)newMaterial;
    _TRACE("PBR material rmaos: {}",
           pbrMaterial->rmaosTexture ? "yes" : "no");
    _TRACE("Input file path: {}", pbrMaterial->inputFilePath);
  }

  if (record->transparency.has_value()) {
    newMaterial->materialAlpha = *record->transparency;
  }
  if (record->emitEnabled) {
    if (record->emitColor.has_value()) {
      lightingShader->emissiveColor = GetColorPtr(*record->emitColor);
    }
    lightingShader->emissiveMult =
        record->emitMult.value_or(lightingShader->emissiveMult);
  }
  if (alphaProperty) {
    alphaProperty->SetAlphaBlending(
        record->alphaBlend.value_or(alphaProperty->GetAlphaBlending()));
    alphaProperty->SetAlphaTesting(
        record->alphaTest.value_or(alphaProperty->GetAlphaTesting()));
    alphaProperty->alphaThreshold =
        record->alphaTestThreshold.value_or(alphaProperty->alphaThreshold);
  }

  if (const auto diffuseMap = GetStringPtr(record->diffuseMap)) {
    newMaterial->diffuseTexture =
        RE::NiPointer(textureLoader->LoadTexture(diffuseMap));
  }
  if (const auto normalMap = GetStringPtr(record->normalMap)) {
    newMaterial->normalTexture =
        RE::NiPointer(textureLoader->LoadTexture(normalMap));
  }
  if (const auto specularMap = GetStringPtr(record->specularMap)) {
    newMaterial->specularBackLightingTexture =
        RE::NiPointer(textureLoader->LoadTexture(specularMap));
  }
  if (record->envMapEnabled.value_or(lightingShader->flags.any(
          RE::BSShaderProperty::EShaderPropertyFlag::kEnvMap))) {
    auto* envMapMaterial = (RE::BSLightingShaderMaterialEnvmap*)newMaterial;
    if (const auto envMap = GetStringPtr(record->envMap)) {
      envMapMaterial->envTexture =
          RE::NiPointer(textureLoader->LoadTexture(envMap));
    }
    if (const auto envMapMask = GetStringPtr(record->envMapMask)) {
      envMapMaterial->envMaskTexture =
          RE::NiPointer(textureLoader->LoadTexture(envMapMask));
    }
  }
  if (record->glowMapEnabled.value_or(lightingShader->flags.any(
          RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap))) {
    auto* glowMapMaterial = (RE::BSLightingShaderMaterialGlowmap*)newMaterial;
    if (const auto glowMap = GetStringPtr(record->glowMap)) {
      glowMapMaterial->glowTexture =
          RE::NiPointer(textureLoader->LoadTexture(glowMap));
    }
  }
  if (record->facegen.value_or(false)) {
    auto* faceGenMaterial = (RE::BSLightingShaderMaterialFacegen*)newMaterial;
    if (const auto faceTintMap = GetStringPtr(record->faceTintMap)) {
      faceGenMaterial->tintTexture =
          RE::NiPointer(textureLoader->LoadTexture(faceTintMap));
    }
    if (const auto detailMap = GetStringPtr(record->detailMap)) {
      faceGenMaterial->detailTexture =
          RE::NiPointer(textureLoader->LoadTexture(detailMap));
    }
    if (const auto subsurfaceMap = GetStringPtr(record->subsurfaceMap)) {
      faceGenMaterial->subsurfaceTexture =
          RE::NiPointer(textureLoader->LoadTexture(subsurfaceMap));
    }
  }
  if (const auto colorMap = GetStringPtr(record->colorBlendMap)) {
    const auto colorBlendTexture =
        RE::NiPointer(textureLoader->LoadTexture(colorMap));
    const auto colorBlendMode =
        record->colorBlendMode.value_or(ColorBlendMode::kMultiply);
    const auto color = record->color.has_value()
                           ? (int)(record->color->at(0) * 255) << 24 |
                                 (int)record->color->at(1) * 255 << 16 |
                                 (int)record->color->at(2) * 255 << 8 |
                                 (int)(record->color->at(3) * 255)
                           : 0xFFFFFFFF;
    _WARN("Not implemented: ApplyColorToTexture");
  }

  if (record->uvOffset.has_value()) {
    newMaterial->texCoordOffset[0] = GetPoint2(*record->uvOffset);
  }
  if (record->uvScale.has_value()) {
    newMaterial->texCoordScale[0] = GetPoint2(*record->uvScale);
  }
  if (record->specularEnabled) {
    if (record->specularPower.has_value()) {
      newMaterial->specularPower = *record->specularPower;
    }
    newMaterial->refractionPower =
        record->refractionPower.value_or(newMaterial->refractionPower);
    newMaterial->specularColorScale =
        record->specularMult.value_or(newMaterial->specularColorScale);
    if (record->specularColor.has_value()) {
      newMaterial->specularColor = GetColor(*record->specularColor);
    }
  }
  if (record->rimLighting) {
    newMaterial->rimLightPower =
        record->rimPower.value_or(newMaterial->rimLightPower);
  }

  lightingShader->SetMaterial(newMaterial, true);
  lightingShader->SetupGeometry(bsTriShape);
  lightingShader->FinishSetupGeometry(bsTriShape);
  return true;
}

static bool ApplyMaterialToRefr(RE::TESObjectREFR* refr,
                                const MaterialConfig* material) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(material)
  auto* refrModel = refr->Get3D();
  RETURN_IF_FALSE(refrModel);
  for (const auto& [shapeName, materialName] : material->applies) {
    auto* niAv = refrModel->GetObjectByName(RE::BSString(shapeName));
    if (!niAv) {
      _WARN("No object found for shape name: {}", shapeName);
      continue;
    }
    auto* geometry = niAv->AsGeometry();
    if (!geometry) {
      _WARN("No geometry found for shape name: {}", shapeName);
      continue;
    }
    auto* materialFile = MaterialLoader::LoadMaterial(materialName);
    if (!materialFile) {
      _ERROR("Failed to load material file: {}", materialName);
      continue;
    }
    ApplyMaterialToNode(geometry, materialFile);
  }
  return true;
}

}  // namespace MaterialHelpers