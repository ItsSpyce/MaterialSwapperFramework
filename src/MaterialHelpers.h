#pragma once

#include "NifHelpers.h"
#include "Models/MaterialRecord.h"

namespace MaterialHelpers {
using ShaderFlag8 = RE::BSShaderProperty::EShaderPropertyFlag8;

inline void ApplyFlags(RE::BSLightingShaderProperty* lightingShader,
                       const MaterialRecord* record) {
#define MAP_FLAG(_MATERIAL_FLAG, _SHADER_FLAG)                \
  if (record->_MATERIAL_FLAG.has_value()) {                   \
    lightingShader->SetFlags(ShaderFlag8::_SHADER_FLAG,       \
                             record->_MATERIAL_FLAG.value()); \
  }

  MAP_FLAG(decal, kDecal)
  MAP_FLAG(twoSided, kTwoSided)
  MAP_FLAG(envMapEnabled, kEnvMap)
  MAP_FLAG(specularEnabled, kSpecular)
  MAP_FLAG(receiveShadows, kReceiveShadows)
  MAP_FLAG(castShadows, kCastShadows)
  MAP_FLAG(facegen, kFace)
  MAP_FLAG(hair, kHairTint)
  MAP_FLAG(decalNoFade, kNoFade)
  MAP_FLAG(nonOccluder, kNonProjectiveShadows)
  MAP_FLAG(refraction, kRefraction)
  MAP_FLAG(refractionalFalloff, kRefractionFalloff)
  MAP_FLAG(grayscaleToPaletteColor, kGrayscaleToPaletteColor)
  MAP_FLAG(depthWrite, kZBufferWrite)
  MAP_FLAG(depthTest, kZBufferTest)
  MAP_FLAG(subsurfaceLighting, kCharacterLighting)
  MAP_FLAG(envMapEye, kEyeReflect)
  MAP_FLAG(emitEnabled, kOwnEmit)
  MAP_FLAG(pbr, kMenuScreen)
}

inline RE::NiColor* GetColorPtr(const array<float, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return new RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                           colorArr[2] / 255.0f);
  }
  return new RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

inline RE::NiColorA* GetColorAPtr(const array<float, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return new RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                            colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return new RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

inline RE::NiColor GetColor(const array<float, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                       colorArr[2] / 255.0f);
  }
  return RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

inline RE::NiColorA GetColorA(const array<float, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                        colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

inline RE::NiPoint2 GetPoint2(const array<float, 2>& pointArr) {
  return RE::NiPoint2(pointArr[0], pointArr[1]);
}

inline RE::NiPoint3 GetPoint3(const array<float, 3>& pointArr) {
  return RE::NiPoint3(pointArr[0], pointArr[1], pointArr[2]);
}

inline const char* GetStringPtr(const std::optional<std::string>& str) {
  if (str && !str->empty()) {
    return str->c_str();
  }
  return nullptr;
}

inline void ApplyColorToTexture(const RE::NiSourceTexturePtr& texture,
                                const RE::NiSourceTexturePtr& colorMask,
                                const ColorBlendMode blendMode,
                                const RE::NiColorA& color) {
  _TRACE(__FUNCTION__);
  auto* d3dTexture = texture->rendererTexture->texture;
  D3D11_TEXTURE2D_DESC originalDesc;
  d3dTexture->GetDesc(&originalDesc);

  auto* d3dColorMaskTexture = colorMask->rendererTexture->texture;
  D3D11_TEXTURE2D_DESC colorMaskDesc;
  d3dColorMaskTexture->GetDesc(&colorMaskDesc);
  static constexpr auto TEXTURE_BLEND_PS =
      L"Data/SKSE/Plugins/MaterialSwapperFramework/shaders/TextureBlendPS.hlsl";

  float r = color.red, g = color.green, b = color.blue, a = color.alpha;
  auto blend = static_cast<u32>(blendMode);

  // 1. Get D3D11 device/context
  ID3D11Device* device = nullptr;
  ID3D11DeviceContext* context = nullptr;
  d3dTexture->GetDevice(&device);
  if (!device) return;
  device->GetImmediateContext(&context);
  if (!context) {
    device->Release();
    return;
  }

  // 2. Create a render target texture (uncompressed, for shader output)
  D3D11_TEXTURE2D_DESC rtDesc = originalDesc;
  rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  rtDesc.CPUAccessFlags = 0;
  rtDesc.MiscFlags = 0;
  rtDesc.Usage = D3D11_USAGE_DEFAULT;
  rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Uncompressed for render target

  ID3D11Texture2D* renderTargetTex = nullptr;
  HRESULT hr = device->CreateTexture2D(&rtDesc, nullptr, &renderTargetTex);
  if (FAILED(hr)) {
    context->Release();
    device->Release();
    return;
  }

  // 3. Create render target view
  ID3D11RenderTargetView* rtv = nullptr;
  hr = device->CreateRenderTargetView(renderTargetTex, nullptr, &rtv);
  if (FAILED(hr)) {
    renderTargetTex->Release();
    context->Release();
    device->Release();
    return;
  }

  // 4. Create shader resource views for input textures
  ID3D11ShaderResourceView* inputSRV = nullptr;
  ID3D11ShaderResourceView* maskSRV = nullptr;
  device->CreateShaderResourceView(d3dTexture, nullptr, &inputSRV);
  device->CreateShaderResourceView(d3dColorMaskTexture, nullptr, &maskSRV);

  // 5. Create constant buffer for blend params
  struct ColorBlendParams {
    float blendColor[4];
    uint32_t blendMode;
    float pad[3];
  } cbData = {{r, g, b, a}, blend, {0, 0, 0}};

  D3D11_BUFFER_DESC cbDesc = {};
  cbDesc.ByteWidth = sizeof(ColorBlendParams);
  cbDesc.Usage = D3D11_USAGE_DYNAMIC;
  cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  ID3D11Buffer* cb = nullptr;
  device->CreateBuffer(&cbDesc, nullptr, &cb);

  D3D11_MAPPED_SUBRESOURCE mapped = {};
  if (SUCCEEDED(context->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
    memcpy(mapped.pData, &cbData, sizeof(cbData));
    context->Unmap(cb, 0);
  }

  // 6. Create sampler state
  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
  ID3D11SamplerState* sampler = nullptr;
  device->CreateSamplerState(&sampDesc, &sampler);

  // 7. Compile/load the pixel shader
  ID3D11PixelShader* pixelShader = nullptr;
  {
    // Try to load precompiled shader blob, or compile at runtime
    // For brevity, assume a helper exists:
    // CompileOrLoadPixelShader(TEXTURE_BLEND_PS, &pixelShader, device) You may
    // need to implement this with D3DCompileFromFile or similar. Here, we
    // assume pixelShader is valid after this call. Compile or load the pixel
    // shader using D3DCompileFromFile
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(TEXTURE_BLEND_PS, nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                    "main",    // entry point
                                    "ps_5_0",  // target profile
                                    0,         // flags1
                                    0,         // flags2
                                    &psBlob, &errorBlob);
    if (FAILED(hr) || !psBlob) {
      if (errorBlob) {
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
      }
      if (psBlob) psBlob->Release();
      sampler->Release();
      cb->Release();
      maskSRV->Release();
      inputSRV->Release();
      rtv->Release();
      renderTargetTex->Release();
      context->Release();
      device->Release();
      return;
    }
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
                                   psBlob->GetBufferSize(), nullptr,
                                   &pixelShader);
    psBlob->Release();
    if (FAILED(hr) || !pixelShader) {
      sampler->Release();
      cb->Release();
      maskSRV->Release();
      inputSRV->Release();
      rtv->Release();
      renderTargetTex->Release();
      context->Release();
      device->Release();
      return;
    }
  }

  // 8. Set up pipeline
  ID3D11RenderTargetView* oldRTV = nullptr;
  context->OMGetRenderTargets(1, &oldRTV, nullptr);
  context->OMSetRenderTargets(1, &rtv, nullptr);

  D3D11_VIEWPORT vp = {};
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.Width = static_cast<float>(rtDesc.Width);
  vp.Height = static_cast<float>(rtDesc.Height);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  context->RSSetViewports(1, &vp);

  context->PSSetShader(pixelShader, nullptr, 0);
  ID3D11ShaderResourceView* srvs[2] = {inputSRV, maskSRV};
  context->PSSetShaderResources(0, 2, srvs);
  context->PSSetSamplers(0, 1, &sampler);
  context->PSSetConstantBuffers(0, 1, &cb);

  DrawFullScreenQuad(context);

  // 10. Restore previous render target
  context->OMSetRenderTargets(1, &oldRTV, nullptr);
  if (oldRTV) oldRTV->Release();

  // 11. Copy render target to original texture (BC7)
  // First, create a staging texture for readback
  D3D11_TEXTURE2D_DESC stagingDesc = originalDesc;
  stagingDesc.Usage = D3D11_USAGE_STAGING;
  stagingDesc.BindFlags = 0;
  stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  stagingDesc.MiscFlags = 0;
  ID3D11Texture2D* stagingTex = nullptr;
  device->CreateTexture2D(&stagingDesc, nullptr, &stagingTex);

  // Copy render target to staging (convert to BC7)
  // 1. Copy render target to staging (uncompressed)
  context->CopyResource(stagingTex, renderTargetTex);

  // 2. Compress stagingTex to BC7 and copy to d3dTexture
  // Use DirectXTex for compression
  {
    DirectX::ScratchImage srcImage, bc7Image;
    D3D11_MAPPED_SUBRESOURCE mappedSrc = {};
    if (SUCCEEDED(context->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mappedSrc))) {
      srcImage.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, rtDesc.Width,
                            rtDesc.Height, 1, 1);
      memcpy(srcImage.GetPixels(), mappedSrc.pData,
             rtDesc.Width * rtDesc.Height * 4);
      context->Unmap(stagingTex, 0);

      DirectX::Compress(srcImage.GetImages(), srcImage.GetImageCount(),
                        srcImage.GetMetadata(), DXGI_FORMAT_BC7_UNORM,
                        DirectX::TEX_COMPRESS_DEFAULT, 0.5f, bc7Image);

      // Update d3dTexture with BC7 data
      D3D11_BOX box = {};
      box.left = 0;
      box.top = 0;
      box.front = 0;
      box.right = rtDesc.Width;
      box.bottom = rtDesc.Height;
      box.back = 1;
      const auto* bc7Pixels = bc7Image.GetPixels();
      context->UpdateSubresource(d3dTexture, 0, &box, bc7Pixels,
                                 bc7Image.GetImages()->rowPitch, 0);
    }
  }

  // 12. Cleanup
  stagingTex->Release();
  pixelShader->Release();
  sampler->Release();
  cb->Release();
  maskSRV->Release();
  inputSRV->Release();
  rtv->Release();
  renderTargetTex->Release();
  context->Release();
  device->Release();
}


inline bool ApplyMaterialToNode(const RE::TESObjectREFR* refr, RE::BSTriShape* bsTriShape, const MaterialRecord* record) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(bsTriShape)
  auto* lightingShader = NifHelpers::GetShaderProperty(bsTriShape);
  auto* alphaProperty = NifHelpers::GetAlphaProperty(bsTriShape);
  auto* newMaterial = RE::BSLightingShaderMaterialBase::CreateMaterial(
      lightingShader->material->GetFeature());
  if (!newMaterial) {
    _ERROR("Failed to create BSLightingShaderMaterialBase for tri shape: {}",
           bsTriShape->name);
    return false;
  }
  newMaterial->CopyMembers(lightingShader->material);
  ApplyFlags(lightingShader, record);

  if (record->transparency.has_value()) {
    newMaterial->materialAlpha = *record->transparency;
  }
  if (record->emitEnabled) {
    if (record->emitColor.has_value()) {
      lightingShader->emissiveColor =
          GetColorPtr(*record->emitColor);
    }
    lightingShader->emissiveMult =
        record->emitMult.value_or(lightingShader->emissiveMult);
  }
  if (alphaProperty) {
    alphaProperty->SetAlphaBlending(
        record->alphaBlend.value_or(alphaProperty->GetAlphaBlending()));
    alphaProperty->SetAlphaTesting(
        record->alphaTest.value_or(alphaProperty->GetAlphaTesting()));
    alphaProperty->alphaThreshold =
        record->alphaTestThreshold.value_or(alphaProperty->alphaThreshold);
  }
  /*textureSet->SetTexturePath(Texture::kDiffuse, record->diffuseMap);
  textureSet->SetTexturePath(Texture::kNormal, record->normalMap);
  textureSet->SetTexturePath(Texture::kSpecular, record->specularMap);
  textureSet->SetTexturePath(Texture::kEnvironment, record->envMap);
  textureSet->SetTexturePath(Texture::kEnvironmentMask, record->envMapMask);
  textureSet->SetTexturePath(Texture::kGlowMap, record->glowMap);*/
  auto* textureLoader = Graphics::TextureLoader::GetSingleton();

  if (const auto diffuseMap =
          GetStringPtr(record->diffuseMap)) {
    newMaterial->diffuseTexture =
        RE::NiPointer(textureLoader->LoadTexture(diffuseMap));
  }
  if (const auto normalMap = GetStringPtr(record->normalMap)) {
    newMaterial->normalTexture =
        RE::NiPointer(textureLoader->LoadTexture(normalMap));
  }
  if (const auto specularMap =
          GetStringPtr(record->specularMap)) {
    newMaterial->specularBackLightingTexture =
        RE::NiPointer(textureLoader->LoadTexture(specularMap));
  }
  if (record->envMapEnabled.value_or(lightingShader->flags.any(
          RE::BSShaderProperty::EShaderPropertyFlag::kEnvMap))) {
    auto* envMapMaterial =
        skyrim_cast<RE::BSLightingShaderMaterialEnvmap*>(newMaterial);
    if (const auto envMap = GetStringPtr(record->envMap)) {
      envMapMaterial->envTexture =
          RE::NiPointer(textureLoader->LoadTexture(envMap));
    }
    if (const auto envMapMask =
            GetStringPtr(record->envMapMask)) {
      envMapMaterial->envMaskTexture =
          RE::NiPointer(textureLoader->LoadTexture(envMapMask));
    }
    newMaterial = envMapMaterial;
  }
  if (record->glowMapEnabled.value_or(lightingShader->flags.any(
          RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap))) {
    auto* glowMapMaterial =
        skyrim_cast<RE::BSLightingShaderMaterialGlowmap*>(newMaterial);
    if (const auto glowMap = GetStringPtr(record->glowMap)) {
      glowMapMaterial->glowTexture =
          RE::NiPointer(textureLoader->LoadTexture(glowMap));
    }
    newMaterial = glowMapMaterial;
  }
  if (const auto colorMap =
          GetStringPtr(record->colorBlendMap)) {
    const auto colorBlendTexture =
        RE::NiPointer(textureLoader->LoadTexture(colorMap));
    const auto colorBlendMode =
        record->colorBlendMode.value_or(ColorBlendMode::kMultiply);
    const auto color = record->color.has_value()
                           ? GetColorA(*record->color)
                           : RE::NiColorA(1.0f, 1.0f, 1.0f, 0.0f);
    ApplyColorToTexture(newMaterial->diffuseTexture, colorBlendTexture,
                        colorBlendMode, color);
  }

  if (record->uvOffset.has_value()) {
    newMaterial->texCoordOffset[0] =
        GetPoint2(*record->uvOffset);
  }
  if (record->uvScale.has_value()) {
    newMaterial->texCoordScale[0] =
        GetPoint2(*record->uvScale);
  }
  if (record->specularEnabled) {
    if (record->specularPower.has_value()) {
      newMaterial->specularPower = *record->specularPower;
    }
    newMaterial->refractionPower =
        record->refractionPower.value_or(newMaterial->refractionPower);
    newMaterial->specularColorScale =
        record->specularMult.value_or(newMaterial->specularColorScale);
    if (record->specularColor.has_value()) {
      newMaterial->specularColor =
          GetColor(*record->specularColor);
    }
  }
  if (record->rimLighting) {
    newMaterial->rimLightPower =
        record->rimPower.value_or(newMaterial->rimLightPower);
  }

  lightingShader->SetMaterial(newMaterial, true);
  lightingShader->SetupGeometry(bsTriShape);
  lightingShader->FinishSetupGeometry(bsTriShape);
  newMaterial->~BSLightingShaderMaterialBase();
  RE::free(newMaterial);
  return true;
}
}  // namespace MaterialHelpers