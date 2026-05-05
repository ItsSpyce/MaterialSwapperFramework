#include "MaterialManager.h"

#include "CommunityShaders/BSLightingShaderMaterialPBR.h"
#include "Graphics/ShaderManager.h"
#include "IO/MaterialLoader.h"
#include "Helpers/MaterialHelpers.h"
#include "ModState.h"
#include "Models/MaterialRecord.h"
#include "TextureLoader.h"
#include "Models/MaterialConfig.h"

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

static DirectX::XMFLOAT4 FromBytes(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return {(float)r / 255.f, (float)g / 255.f, (float)b / 255.f,
          (float)a / 255.f};
}

static void ApplyNonPBRTextures(const ShaderProperty* lightingShader,
                                MaterialBase* newMaterial,
                                const MaterialRecord* record) {
  auto* textureLoader = Graphics::TextureLoader::GetSingleton();
  if (const auto diffuseMap = GetStringPtr(record->diffuseMap)) {
    auto diffuseTexture = textureLoader->LoadTexture(diffuseMap);
    if (record->color.has_value()) {
      auto color = record->color.value();
      auto blendMode = record->colorBlendMode.value_or(ColorBlendMode::kNormal);
      auto blendMask = record->colorBlendMap.value_or("").c_str();
      auto* inSrv = diffuseTexture->rendererTexture->resourceView;
      ComPtr<ID3D11Resource> texResource;
      inSrv->GetResource(&texResource);
      ComPtr<ID3D11Texture2D> inTexture;
      texResource.As(&inTexture);
      auto* oldTexture = diffuseTexture->rendererTexture->texture;
      auto* oldResourceView = diffuseTexture->rendererTexture->resourceView;
      if (Graphics::ShaderManager::GetSingleton()->ApplyPaintBlendShader(
              inTexture.Get(), inSrv, blendMask,
              FromBytes(color[0], color[1], color[2], color[3]),
              (UINT)blendMode, "",
              &diffuseTexture->rendererTexture->resourceView)) {
        ComPtr<ID3D11Resource> paintedResource;
        diffuseTexture->rendererTexture->resourceView->GetResource(
            &paintedResource);
        ComPtr<ID3D11Texture2D> paintedTexture;
        paintedResource.As(&paintedTexture);
        diffuseTexture->rendererTexture->texture = paintedTexture.Detach();
        if (oldTexture) oldTexture->Release();
        if (oldResourceView) oldResourceView->Release();
      }
    }
    newMaterial->diffuseTexture = RE::NiPointer(diffuseTexture);
  }

  if (const auto normalMap = GetStringPtr(record->normalMap)) {
    newMaterial->normalTexture =
        RE::NiPointer(textureLoader->LoadTexture(normalMap));
  }
  if (const auto specularMap = GetStringPtr(record->specularMap);
      specularMap && record->specularEnabled == true) {
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
  bool hasOneLayer = false;
  if (const auto diffuseMap = GetStringPtr(record->diffuseMap)) {
    newMaterial->diffuseTexture =
        RE::NiPointer(textureLoader->LoadTexture(diffuseMap));
  }
  if (record->color.has_value()) {
    auto color = record->color.value();
    auto blendMode = record->colorBlendMode.value_or(ColorBlendMode::kNormal);
    auto blendMask = record->colorBlendMap.value_or("").c_str();
    auto* inTexture =
        (ID3D11Texture2D*)newMaterial->diffuseTexture->rendererTexture->texture;
    auto* inSrv = newMaterial->diffuseTexture->rendererTexture->resourceView;
    auto* oldTexture = newMaterial->diffuseTexture->rendererTexture->texture;
    auto* oldResourceView =
        newMaterial->diffuseTexture->rendererTexture->resourceView;
    if (Graphics::ShaderManager::GetSingleton()->ApplyPaintBlendShader(
            inTexture, inSrv, blendMask,
            DirectX::XMFLOAT4(color[0], color[1], color[2], color[3]),
            (UINT)blendMode, "",
            &newMaterial->diffuseTexture->rendererTexture->resourceView)) {
      ComPtr<ID3D11Resource> paintedResource;
      newMaterial->diffuseTexture->rendererTexture->resourceView->GetResource(
          &paintedResource);
      ComPtr<ID3D11Texture2D> paintedTexture;
      paintedResource.As(&paintedTexture);
      newMaterial->diffuseTexture->rendererTexture->texture =
          paintedTexture.Detach();
      if (oldTexture) oldTexture->Release();
      if (oldResourceView) oldResourceView->Release();
    }
  }
  if (const auto normalMap = GetStringPtr(record->normalMap)) {
    newMaterial->normalTexture =
        RE::NiPointer(textureLoader->LoadTexture(normalMap));
  }
  if (const auto specularMap = GetStringPtr(record->specularMap)) {
    newMaterial->specularBackLightingTexture =
        RE::NiPointer(textureLoader->LoadTexture(specularMap));
  }
  if (const auto smoothSpecMap = GetStringPtr(record->smoothSpecMap)) {
    newMaterial->specularBackLightingTexture =
        RE::NiPointer(textureLoader->LoadTexture(smoothSpecMap));
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
    newMaterial->pbrFlags &= PBRFlags::Subsurface;
  }
  if (const auto coatMap = GetStringPtr(record->coatMap)) {
    if (hasOneLayer) {
      newMaterial->pbrFlags &= PBRFlags::TwoLayer;
    } else {
      hasOneLayer = true;
    }
    newMaterial->featuresTexture0 =
        RE::NiPointer(textureLoader->LoadTexture(coatMap));
    newMaterial->pbrFlags &= PBRFlags::ColoredCoat;
  }
  if (const auto parallaxMap = GetStringPtr(record->parallaxMap)) {
    if (hasOneLayer) {
      newMaterial->pbrFlags &= PBRFlags::TwoLayer;
    } else {
      hasOneLayer = true;
    }
    newMaterial->featuresTexture1 =
        RE::NiPointer(textureLoader->LoadTexture(parallaxMap));
    newMaterial->pbrFlags &= PBRFlags::InterlayerParallax;
  }
  if (const auto coatNormalMap = GetStringPtr(record->coatNormalMap)) {
    if (hasOneLayer) {
      newMaterial->pbrFlags &= PBRFlags::TwoLayer;
    } else {
      hasOneLayer = true;
    }
    newMaterial->featuresTexture1 =
        RE::NiPointer(textureLoader->LoadTexture(coatNormalMap));
    newMaterial->pbrFlags &= PBRFlags::CoatNormal;
  }
  if (const auto fuzzMap = GetStringPtr(record->fuzzMap)) {
    newMaterial->featuresTexture1 =
        RE::NiPointer(textureLoader->LoadTexture(fuzzMap));
    newMaterial->pbrFlags &= PBRFlags::Fuzz;
  }
  if (const auto emissionMap = GetStringPtr(record->emissionMap)) {
    newMaterial->emissiveTexture =
        RE::NiPointer(textureLoader->LoadTexture(emissionMap));
  }
}

static void ApplyGlobalFields(RE::BSLightingShaderMaterialBase* mat,
                              const MaterialRecord* record) {
  if (record->specularEnabled == true) {
    mat->specularColor = record->specularColor
                             .transform([](const array<float, 3> col) {
                               return RE::NiColor(col[0], col[1], col[2]);
                             })
                             .value_or(RE::NiColor());
    mat->specularColorScale =
        record->specularMult.value_or(mat->specularColorScale);
    mat->specularPower = record->specularPower.value_or(mat->specularPower);
  }
  mat->rimLightPower = record->rimPower.value_or(mat->rimLightPower);
  mat->materialAlpha = record->transparency.value_or(mat->materialAlpha);
  mat->texCoordScale[0] = record->uvScale
                              .transform([](const array<float, 2> scale) {
                                return RE::NiPoint2(scale[0], scale[1]);
                              })
                              .value_or(mat->texCoordScale[0]);
  mat->texCoordOffset[0] = record->uvOffset
                               .transform([](const array<float, 2> offset) {
                                 return RE::NiPoint2(offset[0], offset[1]);
                               })
                               .value_or(mat->texCoordOffset[0]);
  mat->refractionPower = record->refractionPower.value_or(mat->refractionPower);
  mat->textureClampMode = record->clamp
                              .transform([](const uint32_t val) {
                                return static_cast<int32_t>(val);
                              })
                              .value_or(mat->textureClampMode);
}

struct DefaultMaterial {
  RE::BSLightingShaderProperty* lightingProperty;
  RE::NiAlphaProperty* alphaProperty;
};

bool MaterialManager::ApplyMaterialToNode(RE::BSGeometry* geometry,
                                          const MaterialRecord* record) {
  static int32_t IDs = INT32_MIN;
  static emhash8::HashMap<UINT64, DefaultMaterial> defaultMaterials;

  RETURN_IF_FALSE(geometry)
  auto& shapeProperty = geometry->GetGeometryRuntimeData().shaderProperty;
  auto* lightingShader = geometry->lightingShaderProp_cast();
  RETURN_IF_FALSE(lightingShader)
  auto* alphaProperty = shapeProperty ? geometry->GetGeometryRuntimeData().alphaProperty.get() : nullptr;
  if (auto* extraData = static_cast<RE::NiIntegerExtraData*>(
          geometry->GetExtraData("MSF_Default"));
      extraData) {
    const auto id = ++IDs;
    extraData = RE::NiIntegerExtraData::Create("MSF_Default", id);
    geometry->AddExtraData("MSF_Default", extraData);
    defaultMaterials[id] = DefaultMaterial{
        .lightingProperty = lightingShader,
        .alphaProperty = alphaProperty,
    };
  } else if (!record) {
    // reset to what's in the map
    auto& [lightingProperty, alphaProperty] =
        defaultMaterials[extraData->value];
    geometry->GetGeometryRuntimeData().alphaProperty.reset(alphaProperty);
    geometry->GetGeometryRuntimeData().shaderProperty.reset(lightingProperty);
    lightingProperty->SetupGeometry(geometry);
    lightingProperty->FinishSetupGeometry(geometry);
    geometry->SetMaterialNeedsUpdate(true);
    return true;
  }
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

  if ((lightingShader->flags.any(ShaderFlag::kMenuScreen) || record->pbr) &&
      ModState::GetSingleton()->IsCSInstalled()) {
    auto* pbrMaterial =
        (BSLightingShaderMaterialPBR*)lightingShader->material->Create();
    RETURN_IF_FALSE(pbrMaterial)
    _TRACE("Applying PBR material to shape {}", geometry->name.c_str());
    // lightingShader->flags &= ShaderFlag::kMenuScreen;
    pbrMaterial->CopyMembers(lightingShader->material);
    ApplyPBRTextures(lightingShader, pbrMaterial, record);
    ApplyGlobalFields(pbrMaterial, record);
    lightingShader->SetMaterial(pbrMaterial, true);
  } else {
    _TRACE("Applying Complex material to shape {}", geometry->name.c_str());
    // lightingShader->flags |= ShaderFlag::kMenuScreen;
    auto* newMaterial =
        skyrim_cast<MaterialBase*>(lightingShader->GetBaseMaterial());
    newMaterial->CopyMembers(lightingShader->material);
    ApplyNonPBRTextures(lightingShader, newMaterial, record);
    ApplyGlobalFields(newMaterial, record);
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
