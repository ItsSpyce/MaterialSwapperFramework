#include "MaterialManager.h"

#include "CommunityShaders/BSLightingShaderMaterialPBR.h"
#include "IO/MaterialLoader.h"
#include "MaterialHelpers.h"
#include "ModState.h"
#include "Models/MaterialRecord.h"
#include "NiOverride.h"
#include "NifHelpers.h"
#include "RE/BSLightingShaderMaterialDynamic.h"
#include "ShaderManager.h"
#include "TextureLoader.h"

using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;
using Texture = RE::BSTextureSet::Texture;
using ShaderProperty = RE::BSLightingShaderProperty;
using EffectProperty = RE::BSEffectShaderProperty;
using MaterialBase = RE::BSLightingShaderMaterialBase;

static void ApplyFlags(ShaderProperty* lightingShader,
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

static void CopyMembers(RE::BSShaderMaterial* from, MaterialBase* to) {
  if (auto* fromBase = skyrim_cast<MaterialBase*>(from)) {
    to->CopyBaseMembers(fromBase);
  }
  to->CopyMembers(to);
}

static const char* GetStringPtr(const std::optional<std::string>& str) {
  if (str && !str->empty()) {
    return str->c_str();
  }
  return nullptr;
}

static void ApplyNonPBRTextures(const ShaderProperty* lightingShader,
                                MaterialBase* newMaterial,
                                const MaterialRecord* record) {
  auto* textureLoader = Graphics::TextureLoader::GetSingleton();
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
  if (record->envMapEnabled.value_or(
          lightingShader->flags.any(ShaderFlag::kEnvMap))) {
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
}

static void ApplyPBRTextures(const ShaderProperty* lightingShader,
                             BSLightingShaderMaterialPBR* newMaterial,
                             const MaterialRecord* record) {
  auto* textureLoader = Graphics::TextureLoader::GetSingleton();
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
  if (const auto rmaosMap = GetStringPtr(record->rmaosMap)) {
    newMaterial->rmaosTexture =
        RE::NiPointer(textureLoader->LoadTexture(rmaosMap));
  }
  if (const auto displacementMap = GetStringPtr(record->displacementMap)) {
    newMaterial->displacementTexture =
        RE::NiPointer(textureLoader->LoadTexture(displacementMap));
  }
  if (const auto subsurfaceMap = GetStringPtr(record->subsurfaceMap)) {
    newMaterial->featuresTexture0 =
        RE::NiPointer(textureLoader->LoadTexture(subsurfaceMap));
  }
  if (const auto coatMap = GetStringPtr(record->coatMap)) {
    newMaterial->featuresTexture0 =
        RE::NiPointer(textureLoader->LoadTexture(coatMap));
  }
  if (const auto parallaxMap = GetStringPtr(record->parallaxMap)) {
    newMaterial->featuresTexture1 =
        RE::NiPointer(textureLoader->LoadTexture(parallaxMap));
  }
  if (const auto fuzzMap = GetStringPtr(record->fuzzMap)) {
    newMaterial->featuresTexture1 =
        RE::NiPointer(textureLoader->LoadTexture(fuzzMap));
  }
  if (const auto coatNormalMap = GetStringPtr(record->coatNormalMap)) {
    newMaterial->featuresTexture1 =
        RE::NiPointer(textureLoader->LoadTexture(coatNormalMap));
  }
}

// I have a crazy fuckin idea. What if I hooked into
// BSShaderProperty::SetupGeometry?
bool MaterialManager::ApplyMaterialToNode(RE::BSGeometry* geometry,
                                          const MaterialRecord* record) {
  RETURN_IF_FALSE(geometry)
  auto& shapeProperty = geometry->GetGeometryRuntimeData()
                            .properties[RE::BSGeometry::States::kProperty];
  auto* lightingShader = geometry->lightingShaderProp_cast();
  RETURN_IF_FALSE(lightingShader)
  auto* alphaProperty =
      shapeProperty ? static_cast<RE::NiAlphaProperty*>(shapeProperty.get())
                    : nullptr;
  /* auto* newMaterial =
       new RE::BSLightingShaderMaterialDynamic(lightingShader->material);
   newMaterial->SetMaterial(record);*/
  ApplyFlags(lightingShader, record);

  if (record->emitEnabled) {
    if (record->emitColor.has_value()) {
      lightingShader->emissiveColor =
          MaterialHelpers::GetColorPtr(*record->emitColor);
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

  if (record->pbr && ModState::GetSingleton()->IsCSInstalled()) {
    auto* pbrMaterial = (BSLightingShaderMaterialPBR*)lightingShader->material->Create();
    RETURN_IF_FALSE(pbrMaterial)
    pbrMaterial->CopyMembers(lightingShader->material);
    ApplyPBRTextures(lightingShader, pbrMaterial, record);
    lightingShader->SetMaterial(pbrMaterial, true);
  } else {
    auto* newMaterial =
        skyrim_cast<MaterialBase*>(lightingShader->GetBaseMaterial());
    newMaterial->CopyMembers(lightingShader->material);
    ApplyNonPBRTextures(lightingShader, newMaterial, record);
    lightingShader->SetMaterial(newMaterial, true);
  }
  lightingShader->SetupGeometry(geometry);
  lightingShader->FinishSetupGeometry(geometry);
  geometry->SetMaterialNeedsUpdate(true);

  // newMaterial->OnLoadTextureSet(0, nullptr);

  return true;
}

bool MaterialManager::ApplyMaterialToRefr(RE::TESObjectREFR* refr,
                                          const MaterialConfig* config) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(config)
  auto* refrModel = refr->Get3D();
  RETURN_IF_FALSE(refrModel);
  for (const auto& [shapeName, materialName] : config->applies) {
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
    auto* record = MaterialLoader::LoadMaterial(materialName);
    if (!record) {
      _ERROR("Failed to load material file: {}", materialName);
      continue;
    }
    ApplyMaterialToNode(geometry, record);
  }
  return true;
}