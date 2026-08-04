#pragma once

#include "Types.h"

namespace RE {
class BSLightingShaderMaterialDynamic : public BSLightingShaderMaterialBase {
 public:
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
  static constexpr auto GlossTexture = BSTextureSet::Texture::kGloss;
  static constexpr auto SubsurfaceTintTexture =
      BSTextureSet::Texture::kSubsurfaceTint;
  static constexpr auto DetailTexture = BSTextureSet::Texture::kDetailMap;
  static constexpr auto HeightTexture = BSTextureSet::Texture::kHeight;
  static constexpr auto MultilayerTexture = BSTextureSet::Texture::kMultilayer;

  explicit BSLightingShaderMaterialDynamic(BSShaderMaterial* base)
      : original_(base) {}
  ~BSLightingShaderMaterialDynamic() override = default;

  static BSLightingShaderMaterialDynamic* CreateMaterial(
      const FileID fileID);

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

  BSLightingShaderMaterialBase* CastToUnderlying() const;
  void SetMaterial(FileID materialID);

  NiSourceTexturePtr environmentTexture = NiSourceTexturePtr{};
  NiSourceTexturePtr environmentMaskTexture = NiSourceTexturePtr{};
  NiSourceTexturePtr specularTexture = NiSourceTexturePtr{};
  NiSourceTexturePtr glowTexture = NiSourceTexturePtr{};
  NiSourceTexturePtr colorTexture = NiSourceTexturePtr{};
  NiColorA color = NiColorA();
  ColorBlendMode colorBlendMode{ColorBlendMode::Normal};
  FileID materialID = NULL;

 private:
  BSShaderMaterial* original_;
};
}  // namespace RE