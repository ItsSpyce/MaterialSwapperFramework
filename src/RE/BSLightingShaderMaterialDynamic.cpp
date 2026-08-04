#include "BSLightingShaderMaterialDynamic.h"

#include "Cache/FilenameIDCache.h"
#include "Graphics/TextureLoader.h"
#include "Helpers/MaterialHelpers.h"
#include "MaterialSwapper.h"
#include "Types.h"

using MaterialBase = RE::BSLightingShaderMaterialBase;

namespace {
bool GetTextureMap(const optional<FileID>& fileID, string& out) {
  if (!fileID || fileID == 0) return false;
  out = FilenameIDCache::GetPathForID(fileID.value()).or_else("");
  return "" != out;
}
}  // namespace

namespace RE {
BSShaderMaterial* BSLightingShaderMaterialDynamic::Create() {
  return new BSLightingShaderMaterialDynamic(nullptr);
}

void BSLightingShaderMaterialDynamic::CopyMembers(BSShaderMaterial* that) {
  auto* other = dynamic_cast<MaterialBase*>(that);
  if (!other) {
    return;
  }
  this->specularColor = other->specularColor;
  this->pad44 = other->pad44;
  this->diffuseTexture = other->diffuseTexture;
  this->diffuseRenderTargetSourceIndex = other->diffuseRenderTargetSourceIndex;
  this->pad54 = other->pad54;
  this->normalTexture = other->normalTexture;
  this->rimSoftLightingTexture = other->rimSoftLightingTexture;
  this->specularBackLightingTexture = other->specularBackLightingTexture;
  this->textureClampMode = other->textureClampMode;
  this->pad74 = other->pad74;
  this->textureSet = other->textureSet;
  this->materialAlpha = other->materialAlpha;
  this->refractionPower = other->refractionPower;
  this->specularPower = other->specularPower;
  this->specularColorScale = other->specularColorScale;
  this->subSurfaceLightRolloff = other->subSurfaceLightRolloff;
  this->rimLightPower = other->rimLightPower;
  this->unk98 = other->unk98;
}

u32 BSLightingShaderMaterialDynamic::ComputeCRC32(u32 srcHash) {
  srcHash = MaterialBase::ComputeCRC32(srcHash);
  if (diffuseTexture) {
    srcHash ^= std::hash<std::string>()(diffuseTexture->name.c_str());
  }
  if (normalTexture) {
    srcHash ^= std::hash<std::string>()(normalTexture->name.c_str());
  }
  if (specularBackLightingTexture) {
    srcHash ^=
        std::hash<std::string>()(specularBackLightingTexture->name.c_str());
  }
  if (environmentTexture) {
    srcHash ^= std::hash<std::string>()(environmentTexture->name.c_str());
  }
  if (environmentMaskTexture) {
    srcHash ^= std::hash<std::string>()(environmentMaskTexture->name.c_str());
  }
  if (glowTexture) {
    srcHash ^= std::hash<std::string>()(glowTexture->name.c_str());
  }
  if (colorTexture) {
    srcHash ^= std::hash<std::string>()(colorTexture->name.c_str());
  }
  if (color != NiColorA()) {
    srcHash ^= std::hash<float>()(color.red);
    srcHash ^= std::hash<float>()(color.green);
    srcHash ^= std::hash<float>()(color.blue);
    srcHash ^= std::hash<float>()(color.alpha);
  }
  srcHash ^= static_cast<u32>(colorBlendMode);
  return srcHash;
}

u32 BSLightingShaderMaterialDynamic::GetTextures(NiSourceTexture** textures) {
  u32 i = 0;
  if (diffuseTexture) {
    textures[i++] = diffuseTexture.get();
  }
  if (normalTexture) {
    textures[i++] = normalTexture.get();
  }
  if (specularBackLightingTexture) {
    textures[i++] = specularBackLightingTexture.get();
  }
  if (environmentTexture) {
    textures[i++] = environmentTexture.get();
  }
  if (environmentMaskTexture) {
    textures[i++] = environmentMaskTexture.get();
  }
  if (glowTexture) {
    textures[i++] = glowTexture.get();
  }
  if (colorTexture) {
    textures[i++] = colorTexture.get();
  }
  return i;
}

void BSLightingShaderMaterialDynamic::LoadBinary(NiStream& stream) {
  CastToUnderlying()->LoadBinary(stream);
}

void BSLightingShaderMaterialDynamic::OnLoadTextureSet(
    u64 arg1, BSTextureSet* textureSet) {
  auto* lock = &unk98;
  while (_InterlockedCompareExchange(lock, 1, 0)) {
    Sleep(0);
  }
  _mm_mfence();
  if (textureSet) {
    this->textureSet = NiPointer(textureSet);
  }
  if (this->textureSet) {
    this->textureSet->SetTexture(DiffuseTexture, diffuseTexture);
    this->textureSet->SetTexture(NormalTexture, normalTexture);
    this->textureSet->SetTexture(SpecularTexture, specularTexture);
    this->textureSet->SetTexture(EnvironmentTexture, environmentTexture);
    this->textureSet->SetTexture(EnvironmentMaskTexture,
                                 environmentMaskTexture);
    this->textureSet->SetTexture(GlowTexture, glowTexture);
    this->textureSet->SetTexture(ColorTexture, colorTexture);
  }
  if (lock) {
    *lock = 0;
    _mm_mfence();
  }
}

void BSLightingShaderMaterialDynamic::ClearTextures() {
  MaterialBase::ClearTextures();
  diffuseTexture.reset();
  normalTexture.reset();
  specularBackLightingTexture.reset();
  environmentTexture.reset();
  environmentMaskTexture.reset();
  glowTexture.reset();
  colorTexture.reset();
}

void BSLightingShaderMaterialDynamic::ReceiveValuesFromRootMaterial(
    bool skinned, bool rimLighting, bool softLighting, bool backLighting,
    bool MSN) {
  MaterialBase::ReceiveValuesFromRootMaterial(skinned, rimLighting,
                                              softLighting, backLighting, MSN);
}

BSLightingShaderMaterialDynamic*
BSLightingShaderMaterialDynamic::CreateMaterial(const FileID materialID) {
  auto* base = MaterialBase::CreateMaterial(Feature::kDefault);
  auto* dyn = new BSLightingShaderMaterialDynamic(base);
  dyn->materialID = materialID;
  return dyn;
}

MaterialBase* BSLightingShaderMaterialDynamic::CastToUnderlying() const {
  MaterialBase* base;
  const auto matr = MaterialSwapper::GetMaterialRecord(materialID);
  if (matr->facegen.value_or(false)) {
    base = MaterialBase::CreateMaterial(Feature::kFaceGen);
  } else if (matr->envMapEnabled.value_or(false)) {
    base = MaterialBase::CreateMaterial(Feature::kEnvironmentMap);
  } else if (matr->glowMapEnabled.value_or(false)) {
    base = MaterialBase::CreateMaterial(Feature::kGlowMap);
  } else {
    base = MaterialBase::CreateMaterial(Feature::kHairTint);
  }
  base->CopyMembers(original_);
  return base;
}

void BSLightingShaderMaterialDynamic::SetMaterial(const FileID materialID) {
  this->materialID = materialID;
  if (!materialID) {
    return;
  }
  const auto material = MaterialSwapper::GetMaterialRecord(materialID);
  if (!material) return;
  auto* texLoader = Graphics::TextureLoader::GetSingleton();
  string tex;
  if (GetTextureMap(material.value().diffuseMap, tex)) {
    diffuseTexture = NiPointer(texLoader->LoadTexture(tex));
  }
  if (GetTextureMap(material.value().normalMap, tex)) {
    normalTexture = NiPointer(texLoader->LoadTexture(tex));
  }
  if (GetTextureMap(material.value().specularMap, tex)) {
    specularTexture = NiPointer(texLoader->LoadTexture(tex));
  }
  if (GetTextureMap(material.value().envMap, tex)) {
    environmentTexture = NiPointer(texLoader->LoadTexture(tex));
  }
  if (GetTextureMap(material.value().envMapMask, tex)) {
    environmentMaskTexture = NiPointer(texLoader->LoadTexture(tex));
  }
  if (GetTextureMap(material.value().glowMap, tex)) {
    glowTexture = NiPointer(texLoader->LoadTexture(tex));
  }

  if (material.value().transparency.has_value()) {
    materialAlpha = half::ToFloat32Fast(material.value().transparency.value());
  }
  if (material.value().uv.has_value()) {
    texCoordOffset[0] = material.value().uv->offset();
    texCoordScale[0] = material.value().uv->scale();
  }
  if (material.value().flags.has_value(MaterialFlags::SpecularEnabled)) {
    if (material.value().specularPower.has_value()) {
      specularPower = half::ToFloat32Fast(material.value().specularPower.value());
    }
    if (material.value().refractionPower.has_value()) {
      refractionPower = half::ToFloat32Fast(material.value().refractionPower.value());
    }
    if (material.value().specularMult.has_value()) {
      specularColorScale = half::ToFloat32Fast(material.value().specularMult.value());
    }
    if (material.value().specularColor.has_value()) {
      specularColor = MaterialHelpers::GetColor(material.value().specularColor.value());
    }
  }
  if (material.value().flags.has_value(MaterialFlags::RimLighting)) {
    rimLightPower = material.value().rimPower.value_or(rimLightPower);
  }
}
}  // namespace RE