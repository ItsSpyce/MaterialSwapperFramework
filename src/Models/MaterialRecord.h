#pragma once
#include <array>
#include <optional>
#include <string>

enum class ColorBlendMode : uint8_t {
  kNormal = 0,
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

struct MaterialRGB {
  uint8_t r = 0, g = 0, b = 0;
};

struct MaterialRGBA {
  uint8_t r = 0, g = 0, b = 0, a = 0;
};

struct MaterialTexture {
  string path = "";
};

struct MaterialMaskedTexture {
  string path = "", mask = "";
};

struct MaterialUV {
  array<double, 2> offset{}, scale{};
};

struct MaterialAlpha {
  double transparency = 1.f;
  bool blend = false;
  bool test = false;
  uint8_t threshold = 128;
};

struct MaterialRendering {
  bool twoSided = false, receiveShadows = true, castShadows = true, decal = false;
};

struct MaterialRecord {
  string schemaVersion = "1.0";
  optional<uint8_t> shaderType;

  // flags
  optional<bool> decal;
  optional<bool> twoSided;
  optional<bool> envMapEnabled;
  optional<bool> specularEnabled;
  optional<bool> receiveShadows;
  optional<bool> castShadows;
  optional<bool> facegen;
  optional<bool> hair;
  optional<bool> decalNoFade;
  optional<bool> nonOccluder;
  optional<bool> refraction;
  optional<bool> refractionalFalloff;
  optional<bool> grayscaleToPaletteColor;
  optional<bool> depthWrite;
  optional<bool> depthTest;
  optional<bool> subsurfaceLighting;
  optional<bool> envMapEye;
  optional<bool> emitEnabled;
  optional<bool> pbr;

  optional<string> inherits;
  optional<int> clamp;
  optional<array<double, 2>> uvOffset;
  optional<array<double, 2>> uvScale;
  optional<double> transparency;
  optional<bool> alphaBlend;
  optional<int> sourceBlendMode;
  optional<int> destinationBlendMode;
  optional<unsigned char> alphaTestThreshold;
  optional<bool> alphaTest;
  optional<double> refractionPower;
  optional<double> envMapMaskScale;
  optional<bool> depthBias;
  optional<unsigned char> maskWrites;

  // textures
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
  optional<string> faceTintMap;
  optional<string> detailMap;
  optional<string> subsurfaceMap;
  optional<string> parallaxMap;
  optional<string> rmaosMap;
  optional<string> coatMap;
  optional<string> coatNormalMap;
  optional<string> fuzzMap;
  optional<string> emissionMap;
  optional<string> colorBlendMap;

  optional<bool> enableEditorAlphaThreshold;
  optional<bool> translucency;
  optional<bool> translucencyThickObject;
  optional<bool> translucencyMixAlbedoWithSubsurfaceColor;
  optional<array<double, 4>> translucencySubsurfaceColor;
  optional<double> translucencyTransmissiveScale;
  optional<double> translucencyTurbulence;
  optional<bool> rimLighting;
  optional<double> rimPower;
  optional<double> backLightPower;
  optional<double> specularPower;
  optional<double> subsurfaceLightingRolloff;
  optional<array<double, 3>> specularColor;
  optional<double> specularMult;
  optional<double> smoothness;
  optional<double> fresnelPower;
  optional<double> wetnessControlSpecScale;
  optional<double> wetnessControlSpecPowerScale;
  optional<double> wetnessControlSpecMinvar;
  optional<double> wetnessControlEnvMapScale;
  optional<double> wetnessControlFresnelPower;
  optional<double> wetnessControlMetalness;
  optional<bool> customPorosity;
  optional<double> porosityValue;
  optional<string> rootMaterialPath;
  optional<bool> anisoLighting;
  optional<array<double, 3>> emitColor;
  optional<double> emitMult;
  optional<bool> modelSpaceNormals;
  optional<bool> externalEmit;
  optional<double> lumEmit;
  optional<bool> useAdaptiveEmissive;
  optional<array<double, 3>> adaptiveEmissiveExposureParams;
  optional<bool> backLighting;
  optional<bool> hideSecret;
  optional<bool> dissolveFade;
  optional<bool> assumeShadowmask;
  optional<bool> glowMapEnabled;


  // custom fields
  optional<array<double, 4>> color;
  optional<ColorBlendMode> colorBlendMode;
  optional<array<double, 4>> colorChannelR;
  optional<array<double, 4>> colorChannelG;
  optional<array<double, 4>> colorChannelB;
};