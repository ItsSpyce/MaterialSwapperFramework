#include "MeshBuilder.h"

#include "Cache/FilenameIDCache.h"
#include "CommunityShaders/BSLightingShaderMaterialPBR.h"
#include "Graphics/ShaderManager.h"
#include "Helpers/MaterialHelpers.h"
#include "MaterialSwapper.h"
#include "ModState.h"
#include "Types.h"

namespace {

using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;
using ShaderProperty = RE::BSLightingShaderProperty;
using MaterialBase = RE::BSLightingShaderMaterialBase;

void ApplyFlags(ShaderProperty* lightingShader, const MATR& record) {
#define MAP_FLAG(_MATERIAL_FLAG, _SHADER_FLAG)                                 \
  if (record.flags.has_value(MaterialFlags::_MATERIAL_FLAG)) {                 \
    lightingShader->SetFlags(ShaderFlag8::_SHADER_FLAG,                        \
                             record.flags.get(MaterialFlags::_MATERIAL_FLAG)); \
  }

  MAP_FLAG(Decal, kDecal)
  MAP_FLAG(TwoSided, kTwoSided)
  MAP_FLAG(EnvMapEnabled, kEnvMap)
  MAP_FLAG(SpecularEnabled, kSpecular)
  MAP_FLAG(ReceiveShadows, kReceiveShadows)
  MAP_FLAG(CastShadows, kCastShadows)
  MAP_FLAG(Facegen, kFace)
  MAP_FLAG(Hair, kHairTint)
  MAP_FLAG(DecalNoFade, kNoFade)
  MAP_FLAG(NonOccluder, kNonProjectiveShadows)
  MAP_FLAG(Refraction, kRefraction)
  MAP_FLAG(RefractionFalloff, kRefractionFalloff)
  MAP_FLAG(GrayscaleToPaletteColor, kGrayscaleToPaletteColor)
  MAP_FLAG(DepthWrite, kZBufferWrite)
  MAP_FLAG(DepthTest, kZBufferTest)
  MAP_FLAG(SubsurfaceLighting, kCharacterLighting)
  MAP_FLAG(EnvMapEye, kEyeReflect)
  MAP_FLAG(EmitEnabled, kOwnEmit)
  MAP_FLAG(Pbr, kMenuScreen)
}

const char* TryGetPath(const FileID texture) {
  auto path = FilenameIDCache::GetPathForID(texture);
  if (path.has_value() && !path->empty()) {
    return path->c_str();
  }
  _ERROR("Failed to get texture: {}", path.error());
  return nullptr;
}

RE::NiPointer<RE::NiSourceTexture> LoadTexture(const std::string& path) {
  RE::NiTexturePtr texturePtr;
  RE::GetTexture(path.c_str(), true, texturePtr, false);
  if (!texturePtr) {
    _ERROR("Failed to load texture: {}", path);
    return nullptr;
  }
  auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(&*texturePtr);
  if (!newTexture) {
    _ERROR("Failed to cast texture to NiSourceTexture for map: {}", path);
    return nullptr;
  }
  return RE::NiPointer(newTexture);
}

void ApplyNonPBRTextures(const ShaderProperty* lightingShader,
                         MaterialBase* newMaterial, const MATR& record) {
  if (const auto diffuseMap = TryGetPath(record.diffuseMap)) {
    const auto diffuseTexture = LoadTexture(diffuseMap);
    if (const auto blendMask = TryGetPath(record.colorBlendMap)) {
      _TRACE("Coloring texture");
      auto r = record.colorChannelR.value_or({0, 0, 0, 0});
      auto g = record.colorChannelG.value_or({0, 0, 0, 0});
      auto b = record.colorChannelB.value_or({0, 0, 0, 0});
      auto blendMode = record.colorBlendMode.value_or(ColorBlendMode::Normal);

      ComPtr<ID3D11Texture2D> oldTexture = static_cast<ID3D11Texture2D*>(
          diffuseTexture->rendererTexture->texture);
      ComPtr<ID3D11ShaderResourceView> oldResourceView =
          diffuseTexture->rendererTexture->resourceView;

      if (!oldTexture) {
        if (oldResourceView) {
          ComPtr<ID3D11Resource> texResource;
          oldResourceView->GetResource(&texResource);
          texResource.As(&oldTexture);
        } else {
          _ERROR("Cannot fetch textures from non-existent resources");
          return;
        }
      }

      if (Graphics::ShaderManager::GetSingleton()
              ->ApplyTextureChannelPaintBlendShader(
                  oldTexture.Get(), oldResourceView.Get(), blendMask,
                  DirectX::XMFLOAT4(r[0], r[1], r[2], r[3]) / 255,
                  DirectX::XMFLOAT4(g[0], g[1], g[2], g[3]) / 255,
                  DirectX::XMFLOAT4(b[0], b[1], b[2], b[3]) / 255,
                  (UINT)blendMode, "",
                  &diffuseTexture->rendererTexture->resourceView)) {
        ComPtr<ID3D11Resource> paintedResource;
        diffuseTexture->rendererTexture->resourceView->GetResource(
            &paintedResource);
        ComPtr<ID3D11Texture2D> paintedTexture;
        paintedResource.As(&paintedTexture);
        diffuseTexture->rendererTexture->texture = paintedTexture.Detach();
      } else {
        _ERROR("Failed to apply blended texture");
      }
      RELEASE(oldTexture);
      RELEASE(oldResourceView);
    }
    newMaterial->diffuseTexture = diffuseTexture;
  }

  if (const auto normalMap = TryGetPath(record.normalMap)) {
    newMaterial->normalTexture = LoadTexture(normalMap);
  }
  if (const auto specularMap = TryGetPath(record.specularMap);
      specularMap && record.flags.get(MaterialFlags::SpecularEnabled)) {
    newMaterial->specularBackLightingTexture = LoadTexture(specularMap);
  }
  if (record.flags.value_or(MaterialFlags::EnvMapEnabled,
                            lightingShader->flags.any(ShaderFlag::kEnvMap))) {
    auto* envMapMaterial = (RE::BSLightingShaderMaterialEnvmap*)newMaterial;
    if (const auto envMap = TryGetPath(record.envMap)) {
      envMapMaterial->envTexture = LoadTexture(envMap);
    }
    if (const auto envMapMask = TryGetPath(record.envMapMask)) {
      envMapMaterial->envMaskTexture = LoadTexture(envMapMask);
    }
  }
  if (record.flags.value_or(
          MaterialFlags::GlowMapEnabled,
          lightingShader->flags.any(
              RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap))) {
    auto* glowMapMaterial = (RE::BSLightingShaderMaterialGlowmap*)newMaterial;
    if (const auto glowMap = TryGetPath(record.glowMap)) {
      glowMapMaterial->glowTexture = LoadTexture(glowMap);
    }
  }
  if (record.flags.get(MaterialFlags::Facegen)) {
    auto* faceGenMaterial = (RE::BSLightingShaderMaterialFacegen*)newMaterial;
    if (const auto faceTintMap = TryGetPath(record.faceTintMap)) {
      faceGenMaterial->tintTexture = LoadTexture(faceTintMap);
    }
    if (const auto detailMap = TryGetPath(record.detailMap)) {
      faceGenMaterial->detailTexture = LoadTexture(detailMap);
    }
    if (const auto subsurfaceMap = TryGetPath(record.subsurfaceMap)) {
      faceGenMaterial->subsurfaceTexture = LoadTexture(subsurfaceMap);
    }
  }
}

void ApplyPBRTextures(const ShaderProperty* lightingShader,
                      BSLightingShaderMaterialPBR* newMaterial,
                      const MATR& record) {
  bool hasOneLayer = false;
  if (const auto diffuseMap = TryGetPath(record.diffuseMap)) {
    const auto diffuseTexture = LoadTexture(diffuseMap);
    if (const auto blendMask = TryGetPath(record.colorBlendMap)) {
      _TRACE("Coloring texture");
      auto r = record.colorChannelR.value_or({0, 0, 0, 0});
      auto g = record.colorChannelG.value_or({0, 0, 0, 0});
      auto b = record.colorChannelB.value_or({0, 0, 0, 0});
      auto blendMode = record.colorBlendMode.value_or(ColorBlendMode::Normal);

      ComPtr<ID3D11Texture2D> oldTexture = static_cast<ID3D11Texture2D*>(
          diffuseTexture->rendererTexture->texture);
      ComPtr<ID3D11ShaderResourceView> oldResourceView =
          diffuseTexture->rendererTexture->resourceView;

      if (!oldTexture) {
        if (oldResourceView) {
          ComPtr<ID3D11Resource> texResource;
          oldResourceView->GetResource(&texResource);
          texResource.As(&oldTexture);
        } else {
          _ERROR("Cannot fetch textures from non-existent resources");
          return;
        }
      }

      if (Graphics::ShaderManager::GetSingleton()
              ->ApplyTextureChannelPaintBlendShader(
                  oldTexture.Get(), oldResourceView.Get(), blendMask,
                  DirectX::XMFLOAT4(r[0], r[1], r[2], r[3]) / 255,
                  DirectX::XMFLOAT4(g[0], g[1], g[2], g[3]) / 255,
                  DirectX::XMFLOAT4(b[0], b[1], b[2], b[3]) / 255,
                  (UINT)blendMode, "",
                  &diffuseTexture->rendererTexture->resourceView)) {
        ComPtr<ID3D11Resource> paintedResource;
        diffuseTexture->rendererTexture->resourceView->GetResource(
            &paintedResource);
        ComPtr<ID3D11Texture2D> paintedTexture;
        paintedResource.As(&paintedTexture);
        diffuseTexture->rendererTexture->texture = paintedTexture.Detach();
      } else {
        _ERROR("Failed to apply blended texture");
      }
      RELEASE(oldTexture);
      RELEASE(oldResourceView);
    }
    newMaterial->diffuseTexture = diffuseTexture;
  }
  if (const auto normalMap = TryGetPath(record.normalMap)) {
    newMaterial->normalTexture = LoadTexture(normalMap);
  }
  if (const auto specularMap = TryGetPath(record.specularMap)) {
    newMaterial->specularBackLightingTexture = LoadTexture(specularMap);
  }
  if (const auto smoothSpecMap = TryGetPath(record.smoothSpecMap)) {
    newMaterial->specularBackLightingTexture = LoadTexture(smoothSpecMap);
  }
  if (const auto rmaosMap = TryGetPath(record.rmaosMap)) {
    newMaterial->rmaosTexture = LoadTexture(rmaosMap);
  }
  if (const auto displacementMap = TryGetPath(record.displacementMap)) {
    newMaterial->displacementTexture = LoadTexture(displacementMap);
  }
  if (const auto subsurfaceMap = TryGetPath(record.subsurfaceMap)) {
    newMaterial->featuresTexture0 = LoadTexture(subsurfaceMap);
    newMaterial->pbrFlags &= PBRFlags::Subsurface;
  }
  if (const auto coatMap = TryGetPath(record.coatMap)) {
    if (hasOneLayer) {
      newMaterial->pbrFlags &= PBRFlags::TwoLayer;
    } else {
      hasOneLayer = true;
    }
    newMaterial->featuresTexture0 = LoadTexture(coatMap);
    newMaterial->pbrFlags &= PBRFlags::ColoredCoat;
  }
  if (const auto parallaxMap = TryGetPath(record.parallaxMap)) {
    if (hasOneLayer) {
      newMaterial->pbrFlags &= PBRFlags::TwoLayer;
    } else {
      hasOneLayer = true;
    }
    newMaterial->featuresTexture1 = LoadTexture(parallaxMap);
    newMaterial->pbrFlags &= PBRFlags::InterlayerParallax;
  }
  if (const auto coatNormalMap = TryGetPath(record.coatNormalMap)) {
    if (hasOneLayer) {
      newMaterial->pbrFlags &= PBRFlags::TwoLayer;
    } else {
      hasOneLayer = true;
    }
    newMaterial->featuresTexture1 = LoadTexture(coatNormalMap);
    newMaterial->pbrFlags &= PBRFlags::CoatNormal;
  }
  if (const auto fuzzMap = TryGetPath(record.fuzzMap)) {
    newMaterial->featuresTexture1 = LoadTexture(fuzzMap);
    newMaterial->pbrFlags &= PBRFlags::Fuzz;
  }
  if (const auto emissionMap = TryGetPath(record.emissionMap)) {
    newMaterial->emissiveTexture = LoadTexture(emissionMap);
  }
}

void ApplyGlobalFields(RE::BSLightingShaderMaterialBase* mat,
                       const MATR& record) {
  if (record.flags.get(MaterialFlags::SpecularEnabled)) {
    mat->specularColor = record.specularColor
                             .transform([](const array<double, 3>& col) {
                               return MaterialHelpers::GetColor(col);
                             })
                             .value_or(RE::NiColor());
    mat->specularColorScale =
        record.specularMult.value_or(mat->specularColorScale);
    mat->specularPower = record.specularPower.value_or(mat->specularPower);
  }
  mat->rimLightPower = record.rimPower.value_or(mat->rimLightPower);
  mat->materialAlpha = record.transparency.value_or(mat->materialAlpha);
  mat->texCoordScale[0] = record.uv
                              .transform([](const uv& uv) {
                                auto [scaleX, scaleY] = uv.scale();
                                return RE::NiPoint2(scaleX, scaleY);
                              })
                              .value_or(mat->texCoordScale[0]);
  mat->texCoordOffset[0] = record.uv
                               .transform([](const uv& uv) {
                                 auto [offsetX, offsetY] = uv.offset();
                                 return RE::NiPoint2(offsetX, offsetY);
                               })
                               .value_or(mat->texCoordOffset[0]);
  mat->refractionPower = record.refractionPower.value_or(mat->refractionPower);
  mat->textureClampMode = record.clamp
                              .transform([](const unsigned int val) {
                                return static_cast<int32_t>(val);
                              })
                              .value_or(mat->textureClampMode);
}

bool ApplyMaterialToNode(RE::BSGeometry* geometry, const MATR& record) {
  if UNLIKELY (!geometry) return false;
  auto& shapeProperty = geometry->GetGeometryRuntimeData().shaderProperty;
  auto* lightingShader = geometry->lightingShaderProp_cast();
  if UNLIKELY (!lightingShader) return false;
  auto* alphaProperty =
      shapeProperty ? geometry->GetGeometryRuntimeData().alphaProperty.get()
                    : nullptr;

  // matr checking
  static consteval auto MSF_CURRENT = "msf_current";
  if (auto* appliedMatrID =
          geometry->GetExtraData<RE::NiIntegerExtraData>(MSF_CURRENT)) {
    if (appliedMatrID->value == record.id) {
      return true;
    }
    geometry->RemoveExtraData(MSF_CURRENT);
  } else {
    RE::NiIntegerExtraData msfCurrent{};
    msfCurrent.name = MSF_CURRENT;
    msfCurrent.value = record.id;
    geometry->AddExtraData(&msfCurrent);
  }

  ApplyFlags(lightingShader, record);

  if (record.flags.get(MaterialFlags::EmitEnabled)) {
    if (record.emitColor.has_value()) {
      lightingShader->emissiveColor =
          MaterialHelpers::GetColorPtr(*record.emitColor);
    }
    lightingShader->emissiveMult =
        record.emitMult.value_or(lightingShader->emissiveMult);
  }
  if (alphaProperty) {
    alphaProperty->SetAlphaBlending(record.flags.value_or(
        MaterialFlags::AlphaBlend, alphaProperty->GetAlphaBlending()));
    alphaProperty->SetAlphaTesting(record.flags.value_or(
        MaterialFlags::AlphaTest, alphaProperty->GetAlphaTesting()));
    alphaProperty->alphaThreshold =
        record.alphaTestThreshold.value_or(alphaProperty->alphaThreshold);
  }

  if ((lightingShader->flags.any(ShaderFlag::kMenuScreen) ||
       record.flags.get(MaterialFlags::Pbr)) &&
      ModState::GetSingleton()->IsCSInstalled()) {
    auto* pbrMaterial =
        (BSLightingShaderMaterialPBR*)lightingShader->material->Create();
    if (!pbrMaterial) return false;
    _TRACE("Applying PBR material to shape {}", geometry->name.c_str());
    // lightingShader->flags &= ShaderFlag::kMenuScreen;
    pbrMaterial->CopyMembers(lightingShader->material);
    ApplyPBRTextures(lightingShader, pbrMaterial, record);
    ApplyGlobalFields(pbrMaterial, record);
    lightingShader->SetMaterial(pbrMaterial, true);
    pbrMaterial->~BSLightingShaderMaterialPBR();
    RE::free(pbrMaterial);
  } else {
    _TRACE("Applying Complex material to shape {}", geometry->name.c_str());
    // lightingShader->flags |= ShaderFlag::kMenuScreen;
    auto* newMaterial = RE::BSLightingShaderMaterialBase::CreateMaterial(
        lightingShader->material->GetFeature());
    newMaterial->CopyMembers(lightingShader->material);
    ApplyNonPBRTextures(lightingShader, newMaterial, record);
    ApplyGlobalFields(newMaterial, record);
    lightingShader->SetMaterial(newMaterial, true);
    newMaterial->~BSLightingShaderMaterialBase();
    RE::free(newMaterial);
  }
  lightingShader->SetupGeometry(geometry);
  lightingShader->FinishSetupGeometry(geometry);
  geometry->SetMaterialNeedsUpdate(true);

  // newMaterial->OnLoadTextureSet(0, nullptr);

  return true;
}

RE::NiNode* CloneNiNode(RE::NiNode* og) {
  auto* clone = (RE::NiNode*)og->Clone();
  return clone;
}
}  // namespace

namespace MeshBuilder {
void ApplyMaterialToMesh(RE::NiNode* root, const MATC& matc) {
  for (const auto& [shapeName, matrFileID] : matc.applies) {
    auto* shape = root->GetObjectByName(shapeName);
    if (!shape) continue;
    const auto matr = MaterialSwapper::GetMaterialRecord(matrFileID);
    if (!matr) {
      _ERROR("Failed to load material record");
      continue;
    }
    if (!ApplyMaterialToNode(shape->AsGeometry(), matr.value())) {
      _ERROR("Failed to apply material to node {}", shapeName);
    }
  }
}
}  // namespace MeshBuilder