#pragma once

namespace RE {
class BSLightingShaderMaterialDynamic : public BSLightingShaderMaterialBase {
 public:
  enum class ColorBlendMode {
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

  static constexpr auto FEATURE = static_cast<Feature>(28);
  static constexpr auto DiffuseTexture = BSTextureSet::Texture::kDiffuse;
  static constexpr auto NormalTexture = BSTextureSet::Texture::kNormal;
  static constexpr auto SpecularTexture = BSTextureSet::Texture::kSpecular;
  static constexpr auto EnvironmentTexture =
      BSTextureSet::Texture::kEnvironment;
  static constexpr auto EnvironmentMaskTexture =
      BSTextureSet::Texture::kEnvironmentMask;
  static constexpr auto GlowTexture = BSTextureSet::Texture::kGlowMap;
  static constexpr auto ColorTexture = BSTextureSet::Texture::kUnused08;

  ~BSLightingShaderMaterialDynamic() = default;

  BSShaderMaterial* Create() override;
  void CopyMembers(BSShaderMaterial* that) override;
  u32 ComputeCRC32(u32 srcHash) override;
  Feature GetFeature() const override { return Feature::kDefault; }
  void OnLoadTextureSet(u64 arg1, BSTextureSet* textureSet) override;
  void ClearTextures() override;
  void ReceiveValuesFromRootMaterial(bool skinned, bool rimLighting,
                                     bool softLighting, bool backLighting,
                                     bool MSN) override;
  u32 GetTextures(NiSourceTexture** textures) override;
  void LoadBinary(NiStream& stream) override;

  NiPointer<NiSourceTexture> environmentTexture;
  NiPointer<NiSourceTexture> environmentMaskTexture;
  NiPointer<NiSourceTexture> glowTexture;
  NiPointer<NiSourceTexture> colorTexture;
  NiColorA* color;
  ColorBlendMode colorBlendMode{ColorBlendMode::kNone};
};
}  // namespace RE