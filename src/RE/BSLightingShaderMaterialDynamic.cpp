#include "BSLightingShaderMaterialDynamic.h"

namespace RE {
BSShaderMaterial* BSLightingShaderMaterialDynamic::Create() {
  return new BSLightingShaderMaterialDynamic;
}

void BSLightingShaderMaterialDynamic::CopyMembers(BSShaderMaterial* that) {
  auto* other = dynamic_cast<BSLightingShaderMaterialDynamic*>(that);
  if (!other) {
    return;
  }
  this->diffuseTexture = other->diffuseTexture;
  this->normalTexture = other->normalTexture;
  this->specularBackLightingTexture = other->specularBackLightingTexture;
  this->environmentTexture = other->environmentTexture;
  this->environmentMaskTexture = other->environmentMaskTexture;
  this->glowTexture = other->glowTexture;
  this->colorTexture = other->colorTexture;
  this->color = other->color;
  this->colorBlendMode = other->colorBlendMode;
}

u32 BSLightingShaderMaterialDynamic::ComputeCRC32(u32 srcHash) {
  // Simple example implementation, you may want to include more members
  srcHash = BSShaderMaterial::ComputeCRC32(srcHash);
  if (diffuseTexture) {
    srcHash ^= std::hash<std::string>()(diffuseTexture->name.c_str());
  }
  if (normalTexture) {
    srcHash ^= std::hash<std::string>()(normalTexture->name.c_str());
  }
  if (specularBackLightingTexture) {
    srcHash ^= std::hash<std::string>()(specularBackLightingTexture->name.c_str());
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
  if (color) {
    srcHash ^= std::hash<float>()(color->red);
    srcHash ^= std::hash<float>()(color->green);
    srcHash ^= std::hash<float>()(color->blue);
    srcHash ^= std::hash<float>()(color->alpha);
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
  BSLightingShaderMaterialBase::LoadBinary(stream);
  // TODO: more fields from binary?
}

void BSLightingShaderMaterialDynamic::OnLoadTextureSet(u64 arg1, BSTextureSet* textureSet) {
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
    this->textureSet->SetTexture(SpecularTexture, specularBackLightingTexture);
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
  diffuseTexture.reset();
  normalTexture.reset();
  specularBackLightingTexture.reset();
  environmentTexture.reset();
  environmentMaskTexture.reset();
  glowTexture.reset();
  colorTexture.reset();
}

void BSLightingShaderMaterialDynamic::ReceiveValuesFromRootMaterial(bool skinned, bool rimLighting, bool softLighting, bool backLighting, bool MSN) {
  BSLightingShaderMaterialBase::ReceiveValuesFromRootMaterial(
      skinned, rimLighting, softLighting, backLighting, MSN);
  // Additional logic for dynamic material if needed
}

}  // namespace RE