#pragma once
#include <array>
#include <optional>
#include <string>

enum class ColorBlendMode : uint8_t {
  kNone = 0,
  kAdd = 1,
  kMultiply = 2,
  kScreen = 3,
  kOverlay = 4,
  kDarken = 5,
  kLighten = 6,
  kColorDodge = 7,
  kColorBurn = 8,
  kHardLight = 9,
  kSoftLight = 10,
};

struct MaterialRecord {
  string schemaVersion = "1.0";
  optional<uint8_t> shaderType;
  optional<string> inherits;
  optional<uint32_t> clamp;
  optional<array<float, 2>> uvOffset;
  optional<array<float, 2>> uvScale;
  optional<float> transparency;
  optional<bool> alphaBlend;
  optional<uint32_t> sourceBlendMode;
  optional<uint32_t> destinationBlendMode;
  optional<uint8_t> alphaTestThreshold;
  optional<bool> alphaTest;
  optional<bool> depthWrite;
  optional<bool> depthTest;
  optional<bool> ssr;
  optional<bool> wetnessControlSsr;
  optional<bool> decal;
  optional<bool> twoSided;
  optional<bool> decalNoFade;
  optional<bool> nonOccluder;
  optional<bool> refraction;
  optional<bool> refractionalFalloff;
  optional<float> refractionPower;
  optional<bool> envMapEnabled;
  optional<float> envMapMaskScale;
  optional<bool> depthBias;
  optional<bool> grayscaleToPaletteColor;
  optional<uint8_t> maskWrites;

  // BGSM specific fields
  optional<string> diffuseMap;
  optional<string> normalMap;
  optional<string> smoothSpecMap;
  optional<string> grayscaleMap;
  optional<string> glowMap;
  optional<string> wrinkleMap;
  optional<string> specularMap;
  optional<string> lightingMap;
  optional<string> flowMap;
  optional<string> distanceFieldAlphaMap;
  optional<string> envMap;
  optional<string> innerLayerMap;
  optional<string> displacementMap;
  optional<string> baseMap;
  optional<string> envMapMask;
  optional<bool> enableEditorAlphaThreshold;
  optional<bool> translucency;
  optional<bool> translucencyThickObject;
  optional<bool> translucencyMixAlbedoWithSubsurfaceColor;
  optional<array<uint8_t, 4>> translucencySubsurfaceColor;
  optional<float> translucencyTransmissiveScale;
  optional<float> translucencyTurbulence;
  optional<bool> rimLighting;
  optional<float> rimPower;
  optional<float> backLightPower;
  optional<float> specularPower;
  optional<bool> subsurfaceLighting;
  optional<float> subsurfaceLightingRolloff;
  optional<bool> specularEnabled;
  optional<array<float, 3>> specularColor;
  optional<float> specularMult;
  optional<float> smoothness;
  optional<float> fresnelPower;
  optional<float> wetnessControlSpecScale;
  optional<float> wetnessControlSpecPowerScale;
  optional<float> wetnessControlSpecMinvar;
  optional<float> wetnessControlEnvMapScale;
  optional<float> wetnessControlFresnelPower;
  optional<float> wetnessControlMetalness;
  optional<bool> pbr;
  optional<bool> customPorosity;
  optional<float> porosityValue;
  optional<string> rootMaterialPath;
  optional<bool> anisoLighting;
  optional<bool> emitEnabled;
  optional<array<float, 3>> emitColor;
  optional<float> emitMult;
  optional<bool> modelSpaceNormals;
  optional<bool> externalEmit;
  optional<float> lumEmit;
  optional<bool> useAdaptiveEmissive;
  optional<array<float, 3>> adaptiveEmissiveExposureParams;
  optional<bool> backLighting;
  optional<bool> receiveShadows;
  optional<bool> hideSecret;
  optional<bool> castShadows;
  optional<bool> dissolveFade;
  optional<bool> assumeShadowmask;
  optional<bool> glowMapEnabled;
  optional<bool> envMapWindow;
  optional<bool> envMapEye;
  optional<bool> hair;
  optional<array<float, 3>> hairTintColor;
  optional<bool> tree;
  optional<bool> facegen;
  optional<bool> skinTint;
  optional<bool> tessellate;
  optional<array<float, 2>> displacementMapParams;
  optional<array<float, 3>> tessellationParams;
  optional<float> grayscaleToPaletteScale;
  optional<bool> skewSpecularAlpha;
  optional<bool> terrain;
  optional<array<float, 3>> terrainParams;

  // BGEM specific fields
  optional<bool> blood;
  optional<bool> effectLighting;
  optional<bool> falloff;
  optional<bool> falloffColor;
  optional<bool> grayscaleToPaletteAlpha;
  optional<bool> soft;
  optional<array<float, 3>> baseColor;
  optional<float> baseColorScale;
  optional<array<float, 4>> falloffParams;
  optional<float> lightingInfluence;
  optional<uint8_t> envMapMinLod;
  optional<float> softDepth;
  optional<bool> effectPbrSpecular;

  // custom fields
  optional<array<float, 4>> color;
  optional<ColorBlendMode> colorBlendMode;
  optional<string> colorBlendMap;
};