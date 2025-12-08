#pragma once

struct GlintParameters {
  bool enabled = false;
  float screenSpaceScale = 1.5f;
  float logMicrofacetDensity = 40.f;
  float microfacetRoughness = .015f;
  float densityRandomization = 2.f;
};

struct TruePBR {
  struct PBRTextureSetData {
    float roughnessScale = 1.f;
    float displacementScale = 1.f;
    float specularLevel = 0.04f;

    RE::NiColor subsurfaceColor;
    float subsurfaceOpacity = 0.f;

    RE::NiColor coatColor = {1.f, 1.f, 1.f};
    float coatStrength = 1.f;
    float coatRoughness = 1.f;
    float coatSpecularLevel = 0.04f;
    float innerLayerDisplacementOffset = 0.f;

    RE::NiColor fuzzColor;
    float fuzzWeight = 0.f;

    GlintParameters glintParameters;
  };
  struct PBRMaterialObjectData {
    std::array<float, 3> baseColorScale = {1.f, 1.f, 1.f};
    float roughness = 1.f;
    float specularLevel = 1.f;

    GlintParameters glintParameters;
  };
};