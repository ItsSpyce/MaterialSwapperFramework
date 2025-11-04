#pragma once

#include "Result.h"
#include "ShaderManager.h"

namespace Graphics {

class TextureManager : public Singleton<TextureManager> {
 public:
  TextureManager() = default;
  ~TextureManager() = default;

  Result<RE::NiSourceTexturePtr> GetNiSourceTexture(const string& filename,
                                                    const string& name) {
#define Ok(...) Result<RE::NiSourceTexturePtr>::Ok(__VA_ARGS__)
#define Err(...) Result<RE::NiSourceTexturePtr>::Err(__VA_ARGS__)

    D3D11_TEXTURE2D_DESC texDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ComPtr<ID3D11Texture2D> texture2d;
    GetTexture2D(filename, texDesc, srvDesc, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0,
                 texture2d);
    if (!texture2d) {
      return Err("Failed to get texture 2d for NiTexture ({})", filename);
    }
    texture2d->GetDesc(&texDesc);
    srvDesc.Format = texDesc.Format;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    ComPtr<ID3D11ShaderResourceView> textureSRV;
    HRESULT hr =
        ShaderManager::GetSingleton()->GetDevice()->CreateShaderResourceView(
            texture2d.Get(), &srvDesc, &textureSRV);
    if (FAILED(hr)) {
      return Err("Failed to create ShaderResourceView for NiTexture ({})", hr);
    }
    RE::NiPointer<RE::NiSourceTexture> output;
    auto result = CreateSourceTexture(name, output);
    if (!result) {
      return Err("Failed to create NiTexture");
    }
    if (*result == CreateSourceTextureResult::kExistingTexture) {
      auto* oldTexture = output->rendererTexture->texture;
      auto* oldResource = output->rendererTexture->resourceView;
      output->rendererTexture->texture = texture2d.Detach();
      output->rendererTexture->resourceView = textureSRV.Detach();
      if (oldTexture) oldTexture->Release();
      if (oldResource) oldResource->Release();
    } else if (*result == CreateSourceTextureResult::kNewTexture) {
      auto* newRendererTexture = new RE::BSGraphics::Texture();
      newRendererTexture->texture = texture2d.Detach();
      newRendererTexture->resourceView = textureSRV.Detach();
      output->rendererTexture = newRendererTexture;
    }
    return Ok(output);
#undef Ok
#undef Err
  }
  Result<> GetTexture2D(const string& filename,
                        D3D11_TEXTURE2D_DESC& textureDesc,
                        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc,
                        DXGI_FORMAT newFormat, UINT newWidth, UINT newHeight,
                        ComPtr<ID3D11Texture2D>& output) {
    return Ok();
  }
  Result<> GetTexture2D(const string& filename,
                        D3D11_TEXTURE2D_DESC& textureDesc,
                        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc,
                        DXGI_FORMAT newFormat,
                        ComPtr<ID3D11Texture2D>& output) {
    return Ok();
  }
  Result<> GetTexture2D(const string& filename,
                        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc,
                        DXGI_FORMAT newFormat,
                        ComPtr<ID3D11Texture2D>& output) {
    return Ok();
  }
  Result<> GetTexture2D(const string& filename,
                        D3D11_TEXTURE2D_DESC& textureDesc,
                        DXGI_FORMAT newFormat,
                        ComPtr<ID3D11Texture2D>& output) {
    return Ok();
  }
  Result<> GetTexture2D(const string& filename, DXGI_FORMAT newFormat,
                        ComPtr<ID3D11Texture2D>& output) {
    return Ok();
  }
  Result<> UpdateTexture(const string& filename) { return Ok(); }

  static RE::NiTexture* CreateTexture(const RE::BSFixedString& name) {
    using func_t = decltype(&CreateTexture);
    REL::VariantID offset(69335, 70717, 0x00CAEF60);
    REL::Relocation<func_t> func{offset};
    return func(name);
  }

  enum class CreateSourceTextureResult {
    kExistingTexture = 0,
    kNewTexture = 1
  };

  static Result<CreateSourceTextureResult> CreateSourceTexture(
      const string& name, RE::NiSourceTexturePtr& out) {
    if (const auto found = GetSingleton()->sourceTextureCache_.find(name);
        found != GetSingleton()->sourceTextureCache_.end()) {
      out = found->second;
      return Ok(CreateSourceTextureResult::kExistingTexture);
    }
    auto* newTex = CreateTexture(name);
    if (!newTex) {
      return Err<CreateSourceTextureResult>(
          "Failed to create NiSourceTexture for name: {}", name);
    }
    RE::NiSourceTexturePtr newSourceTexture(
        static_cast<RE::NiSourceTexture*>(newTex));
    newSourceTexture->name = name;
    GetSingleton()->sourceTextureCache_[name] = newSourceTexture;
    out = newSourceTexture;
    return Ok(CreateSourceTextureResult::kNewTexture);
  }

  static void LoadTexture(const char* path, u8 unk1,
                          RE::NiSourceTexturePtr& texture, bool unk2) {
    using func_t = decltype(&LoadTexture);
    REL::Relocation<func_t> func{RELOCATION_ID(98986, 105640)};
    return func(path, unk1, texture, unk2);
  }

  static u32 InitializeShader(RE::BSLightingShaderProperty* shaderProp,
                              RE::BSGeometry* geometry) {
    using func_t = decltype(&InitializeShader);
    REL::VariantID offset(99862, 106507, 0x01303AC0);
    REL::Relocation<func_t> func{offset};
    return func(shaderProp, geometry);
  }

  static void InvalidateTextures(RE::BSLightingShaderProperty* shaderProp,
                                 u32 unk1) {
    using func_t = decltype(&InvalidateTextures);
    REL::VariantID offset(99865, 106510, 0x01303EA0);
    REL::Relocation<func_t> func{offset};
    return func(shaderProp, unk1);
  }

  Result<> ConvertTexture(ComPtr<ID3D11Texture2D> texture,
                          DXGI_FORMAT newFormat, UINT newWidth, UINT newHeight,
                          DirectX::TEX_FILTER_FLAGS resizeFilter,
                          ComPtr<ID3D11Texture2D>& output) {
    return Ok();
  }
  Result<> CopyTexture(ComPtr<ID3D11Texture2D>& srcTexture,
                       ComPtr<ID3D11Texture2D>& dstTexture) {
    return Ok();
  }
  Result<> CompressTexture(ComPtr<ID3D11Texture2D> srcTexture,
                           DXGI_FORMAT newFormat,
                           ComPtr<ID3D11Texture2D> dstTexture) {
    return Ok();
  }
  Result<> GetTextureFromFile(std::string filePath,
                              ComPtr<ID3D11Texture2D>& texture,
                              ComPtr<ID3D11ShaderResourceView>& srv) {
    return Ok();
  }

  enum class CompressionTarget { kNone, kCPU, kGPU };
  CompressionTarget GetCompressionTarget(DXGI_FORMAT format) {
    return CompressionTarget::kNone;
  }

  Result<> CreateNiTexture(const string&& name, ComPtr<ID3D11Texture2D> dstTex,
                           ComPtr<ID3D11ShaderResourceView> dstSRV,
                           RE::NiSourceTexturePtr& output) {
    return Ok();
  }

 private:
  concurrency::concurrent_unordered_map<string, RE::NiSourceTexturePtr>
      sourceTextureCache_;
};
}  // namespace Graphics