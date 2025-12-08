#pragma once

#include "TruePBR.h"

enum class PBRFlags : uint32_t {
  Subsurface = 1 << 0,
  TwoLayer = 1 << 1,
  ColoredCoat = 1 << 2,
  InterlayerParallax = 1 << 3,
  CoatNormal = 1 << 4,
  Fuzz = 1 << 5,
  HairMarschner = 1 << 6,
};

enum class PBRShaderFlags : uint32_t {
  HasEmissive = 1 << 0,
  HasDisplacement = 1 << 1,
  HasFeaturesTexture0 = 1 << 2,
  HasFeaturesTexture1 = 1 << 3,
  Subsurface = 1 << 4,
  TwoLayer = 1 << 5,
  ColoredCoat = 1 << 6,
  InterlayerParallax = 1 << 7,
  CoatNormal = 1 << 8,
  Fuzz = 1 << 9,
  HairMarschner = 1 << 10,
  Glint = 1 << 11,
  ProjectedGlint = 1 << 12,
};

class BSLightingShaderMaterialPBR : public RE::BSLightingShaderMaterialBase {
 public:
  struct MaterialExtensions {
    TruePBR::PBRTextureSetData* textureSetData = nullptr;
    TruePBR::PBRMaterialObjectData* materialObjectData = nullptr;
  };
  inline static constexpr auto FEATURE = static_cast<RE::BSShaderMaterial::Feature>(32);

  ~BSLightingShaderMaterialPBR() = 0;

  // override (BSLightingShaderMaterialBase)
  RE::BSShaderMaterial* Create() override = 0;                // 01
  void CopyMembers(RE::BSShaderMaterial* that) override = 0;  // 02
  std::uint32_t ComputeCRC32(uint32_t srcHash) override = 0;  // 04
  Feature GetFeature() const override = 0;                    // 06
  void OnLoadTextureSet(std::uint64_t arg1,
                        RE::BSTextureSet* inTextureSet) override = 0;  // 08
  void ClearTextures() override = 0;                                   // 09
  void ReceiveValuesFromRootMaterial(bool skinned, bool rimLighting,
                                     bool softLighting, bool backLighting,
                                     bool MSN) override = 0;          // 0A
  uint32_t GetTextures(RE::NiSourceTexture** textures) override = 0;  // 0B
  void LoadBinary(RE::NiStream& stream) override = 0;                 // 0D

  // members
  RE::BSShaderMaterial::Feature loadedWithFeature;

  stl::enumeration<PBRFlags> pbrFlags;

  float coatRoughness;
  float coatSpecularLevel;

  RE::NiColor fuzzColor;
  float fuzzWeight;

  GlintParameters glintParameters;

  // Roughness in r, metallic in g, AO in b, nonmetal reflectance in a
  RE::NiPointer<RE::NiSourceTexture> rmaosTexture;

  // Emission color in rgb
  RE::NiPointer<RE::NiSourceTexture> emissiveTexture;

  // Displacement in r
  RE::NiPointer<RE::NiSourceTexture> displacementTexture;

  // Subsurface map (subsurface color in rgb, thickness in a) / Coat map (coat
  // color in rgb, coat strength in a)
  RE::NiPointer<RE::NiSourceTexture> featuresTexture0;

  // Fuzz map (fuzz color in rgb, fuzz weight in a) / Coat normal map (coat
  // normal in rgb, coat roughness in a)
  RE::NiPointer<RE::NiSourceTexture> featuresTexture1;

  std::array<float, 3> projectedMaterialBaseColorScale;
  float projectedMaterialRoughness;
  float projectedMaterialSpecularLevel;
  GlintParameters projectedMaterialGlintParameters;
  std::string inputFilePath;
};