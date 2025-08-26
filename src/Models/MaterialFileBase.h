#pragma once

#include "IO/MaterialStream.h"
#include <glaze/glaze.hpp>

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
};