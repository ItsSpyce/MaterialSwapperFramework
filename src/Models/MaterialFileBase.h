#pragma once
#include "Helpers.h"
#include "IO/MaterialStream.h"

struct MaterialFileBase;

enum class ShaderType {
  kLighting,
  kEffect,
};

struct BGSMFile;
struct BGEMFile;

struct MaterialFileBase {
  ShaderType shaderType;
  uint32_t version;
  uint32_t clamp;
  Vector2 uvOffset, uvScale;
  float transparency;
  bool alphaBlend;
  uint32_t sourceBlendMode;
  uint32_t destinationBlendMode;
  uint8_t alphaTestThreshold;
  bool alphaTest;
  bool depthWrite;
  bool depthTest;
  bool ssr;
  bool wetnessControlSsr;
  bool decal;
  bool twoSided;
  bool decalNoFade;
  bool nonOccluder;
  bool refraction;
  bool refractionalFalloff;
  float refractionPower;
  bool envMapEnabled;
  float envMapMaskScale;
  bool depthBias;
  bool grayscaleToPaletteColor;
  uint8_t maskWrites;

  MaterialFileBase() = default;
  virtual void Read(MaterialStream& stream);
  virtual ~MaterialFileBase() = default;
  _NODISCARD virtual size_t GetHashCode() const {
    size_t hash = 0;
    uint16_t flags = 0;
    flags |= (alphaBlend ? 1 : 0) << 0;
    flags |= (depthWrite ? 1 : 0) << 1;
    flags |= (depthTest ? 1 : 0) << 2;
    flags |= (ssr ? 1 : 0) << 3;
    flags |= (wetnessControlSsr ? 1 : 0) << 4;
    flags |= (decal ? 1 : 0) << 5;
    flags |= (twoSided ? 1 : 0) << 6;
    flags |= (decalNoFade ? 1 : 0) << 7;
    flags |= (nonOccluder ? 1 : 0) << 8;
    flags |= (refraction ? 1 : 0) << 9;
    flags |= (refractionalFalloff ? 1 : 0) << 10;
    flags |= (envMapEnabled ? 1 : 0) << 11;
    flags |= (depthBias ? 1 : 0) << 12;
    flags |= (grayscaleToPaletteColor ? 1 : 0) << 13;
    flags |= (alphaTest ? 1 : 0) << 14;
    Helpers::HashCombine(hash, flags);
    Helpers::HashCombine(hash, shaderType);
    Helpers::HashCombine(hash, version);
    Helpers::HashCombine(hash, clamp);
    Helpers::HashCombine(hash, uvOffset);
    Helpers::HashCombine(hash, uvScale);
    Helpers::HashCombine(hash, transparency);
    Helpers::HashCombine(hash, sourceBlendMode);
    Helpers::HashCombine(hash, destinationBlendMode);
    Helpers::HashCombine(hash, alphaTestThreshold);
    Helpers::HashCombine(hash, refractionPower);
    Helpers::HashCombine(hash, envMapMaskScale);
    Helpers::HashCombine(hash, maskWrites);

    return hash;
  }

  template <typename T>
  _NODISCARD T* As() {
    return dynamic_cast<T*>(this);
  }
};

struct BGSMFile : MaterialFileBase {
  std::string diffuseMap;
  std::string normalMap;
  std::string smoothSpecMap;
  std::string grayscaleMap;
  std::string glowMap;
  std::string wrinkleMap;
  std::string specularMap;
  std::string lightingMap;
  std::string flowMap;
  std::string distanceFieldAlphaMap;
  std::string envMap;
  std::string innerLayerMap;
  std::string displacementMap;
  bool enableEditorAlphaThreshold;
  bool translucency;
  bool translucencyThickObject;
  bool translucencyMixAlbedoWithSubsurfaceColor;
  Vector3 translucencySubsurfaceColor;
  float translucencyTransmissiveScale;
  float translucencyTurbulence;
  bool rimLighting;
  float rimPower;
  float backLightPower;
  bool subsurfaceLighting;
  float subsurfaceLightingRolloff;
  bool specularEnabled;
  Vector3 specularColor;
  float specularMult;
  float smoothness;
  float fresnelPower;
  float wetnessControlSpecScale;
  float wetnessControlSpecPowerScale;
  float wetnessControlSpecMinvar;
  float wetnessControlEnvMapScale;
  float wetnessControlFresnelPower;
  float wetnessControlMetalness;
  bool pbr;
  bool customPorosity;
  float porosityValue;
  std::string rootMaterialPath;
  bool anisoLighting;
  bool emitEnabled;
  Vector3 emitColor;
  float emitMult;
  bool modelSpaceNormals;
  bool externalEmit;
  float lumEmit;
  bool useAdaptiveEmissive;
  Vector3 adaptiveEmissiveExposureParams;
  bool backLighting;
  bool receiveShadows;
  bool hideSecret;
  bool castShadows;
  bool dissolveFade;
  bool assumeShadowmask;
  bool glowMapEnabled;
  bool envMapWindow;
  bool envMapEye;
  bool hair;
  Vector3 hairTintColor;
  bool tree;
  bool facegen;
  bool skinTint;
  bool tessellate;
  Vector2 displacementMapParams;
  Vector3 tessellationParams;
  float grayscaleToPaletteScale;
  bool skewSpecularAlpha;
  bool terrain;
  Vector3 terrainParams;

  void Read(MaterialStream& stream) override;
  size_t GetHashCode() const override {
    auto orig = MaterialFileBase::GetHashCode();
    uint32_t flags = 0;
    flags |= (enableEditorAlphaThreshold ? 1 : 0) << 0;
    flags |= (translucency ? 1 : 0) << 1;
    flags |= (translucencyThickObject ? 1 : 0) << 2;
    flags |= (translucencyMixAlbedoWithSubsurfaceColor ? 1 : 0) << 3;
    flags |= (rimLighting ? 1 : 0) << 4;
    flags |= (subsurfaceLighting ? 1 : 0) << 5;
    flags |= (specularEnabled ? 1 : 0) << 6;
    flags |= (pbr ? 1 : 0) << 7;
    flags |= (customPorosity ? 1 : 0) << 8;
    flags |= (anisoLighting ? 1 : 0) << 9;
    flags |= (emitEnabled ? 1 : 0) << 10;
    flags |= (modelSpaceNormals ? 1 : 0) << 11;
    flags |= (externalEmit ? 1 : 0) << 12;
    flags |= (useAdaptiveEmissive ? 1 : 0) << 13;
    flags |= (backLighting ? 1 : 0) << 14;
    flags |= (receiveShadows ? 1 : 0) << 15;
    flags |= (hideSecret ? 1 : 0) << 16;
    flags |= (castShadows ? 1 : 0) << 17;
    flags |= (dissolveFade ? 1 : 0) << 18;
    flags |= (assumeShadowmask ? 1 : 0) << 19;
    flags |= (glowMapEnabled ? 1 : 0) << 20;
    flags |= (envMapWindow ? 1 : 0) << 21;
    flags |= (envMapEye ? 1 : 0) << 22;
    flags |= (hair ? 1 : 0) << 23;
    flags |= (tree ? 1 : 0) << 24;
    flags |= (facegen ? 1 : 0) << 25;
    flags |= (skinTint ? 1 : 0) << 26;
    flags |= (tessellate ? 1 : 0) << 27;
    flags |= (skewSpecularAlpha ? 1 : 0) << 28;
    flags |= (terrain ? 1 : 0) << 29;
    Helpers::HashCombine(orig, flags);
    Helpers::HashCombine(orig, diffuseMap);
    Helpers::HashCombine(orig, normalMap);
    Helpers::HashCombine(orig, smoothSpecMap);
    Helpers::HashCombine(orig, grayscaleMap);
    Helpers::HashCombine(orig, glowMap);
    Helpers::HashCombine(orig, wrinkleMap);
    Helpers::HashCombine(orig, specularMap);
    Helpers::HashCombine(orig, lightingMap);
    Helpers::HashCombine(orig, flowMap);
    Helpers::HashCombine(orig, distanceFieldAlphaMap);
    Helpers::HashCombine(orig, envMap);
    Helpers::HashCombine(orig, innerLayerMap);
    Helpers::HashCombine(orig, displacementMap);
    Helpers::HashCombine(orig, translucencySubsurfaceColor);
    Helpers::HashCombine(orig, translucencyTransmissiveScale);
    Helpers::HashCombine(orig, translucencyTurbulence);
    Helpers::HashCombine(orig, rimPower);
    Helpers::HashCombine(orig, backLightPower);
    Helpers::HashCombine(orig, subsurfaceLightingRolloff);
    Helpers::HashCombine(orig, specularColor);
    Helpers::HashCombine(orig, specularMult);
    Helpers::HashCombine(orig, smoothness);
    Helpers::HashCombine(orig, fresnelPower);
    Helpers::HashCombine(orig, wetnessControlSpecScale);
    Helpers::HashCombine(orig, wetnessControlSpecPowerScale);
    Helpers::HashCombine(orig, wetnessControlSpecMinvar);
    Helpers::HashCombine(orig, wetnessControlEnvMapScale);
    Helpers::HashCombine(orig, wetnessControlFresnelPower);
    Helpers::HashCombine(orig, wetnessControlMetalness);
    Helpers::HashCombine(orig, porosityValue);
    Helpers::HashCombine(orig, rootMaterialPath);
    Helpers::HashCombine(orig, hairTintColor);
    Helpers::HashCombine(orig, emitColor);
    Helpers::HashCombine(orig, emitMult);
    Helpers::HashCombine(orig, lumEmit);
    Helpers::HashCombine(orig, adaptiveEmissiveExposureParams);
    Helpers::HashCombine(orig, displacementMapParams);
    Helpers::HashCombine(orig, tessellationParams);
    Helpers::HashCombine(orig, terrainParams);
    Helpers::HashCombine(orig, grayscaleToPaletteScale);
    return orig;
  }
};

struct BGEMFile : MaterialFileBase {
  std::string baseMap;
  std::string grayscaleMap;
  std::string envMap;
  std::string normalMap;
  std::string envMapMask;
  std::string specularMap;
  std::string lightingMap;
  std::string glowMap;
  bool blood;
  bool effectLighting;
  bool falloff;
  bool falloffColor;
  bool grayscaleToPaletteAlpha;
  bool soft;
  Vector3 baseColor;
  float baseColorScale;
  Vector4 falloffParams;
  float lightingInfluence;
  uint8_t envMapMinLod;
  float softDepth;
  Vector3 emitColor;
  Vector3 adaptiveEmissiveExposureParams;
  bool glowMapEnabled;
  bool effectPbrSpecular;

  void Read(MaterialStream& stream) override;

  size_t GetHashCode() const override {
    auto orig = MaterialFileBase::GetHashCode();
    uint32_t flags = 0;
    flags |= (blood ? 1 : 0) << 0;
    flags |= (effectLighting ? 1 : 0) << 1;
    flags |= (falloff ? 1 : 0) << 2;
    flags |= (falloffColor ? 1 : 0) << 3;
    flags |= (grayscaleToPaletteAlpha ? 1 : 0) << 4;
    flags |= (soft ? 1 : 0) << 5;
    flags |= (glowMapEnabled ? 1 : 0) << 6;
    flags |= (effectPbrSpecular ? 1 : 0) << 7;
    Helpers::HashCombine(orig, flags);
    Helpers::HashCombine(orig, baseMap);
    Helpers::HashCombine(orig, grayscaleMap);
    Helpers::HashCombine(orig, envMap);
    Helpers::HashCombine(orig, normalMap);
    Helpers::HashCombine(orig, envMapMask);
    Helpers::HashCombine(orig, specularMap);
    Helpers::HashCombine(orig, lightingMap);
    Helpers::HashCombine(orig, glowMap);
    Helpers::HashCombine(orig, baseColor);
    Helpers::HashCombine(orig, baseColorScale);
    Helpers::HashCombine(orig, falloffParams);
    Helpers::HashCombine(orig, lightingInfluence);
    Helpers::HashCombine(orig, envMapMinLod);
    Helpers::HashCombine(orig, softDepth);
    Helpers::HashCombine(orig, emitColor);
    Helpers::HashCombine(orig, adaptiveEmissiveExposureParams);
    return orig;
  }
};