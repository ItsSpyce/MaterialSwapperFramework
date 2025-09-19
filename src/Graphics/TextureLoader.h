#pragma once

#include <DirectXTex.h>
#include <d3d11.h>

#include "RE/Misc.h"

namespace Graphics {

class TextureLoader : public Singleton<TextureLoader> {
 public:
  TextureLoader() {}

  RE::NiSourceTexture* LoadTexture(const string& path) {
    RE::NiTexturePtr texturePtr;
    RE::GetTexture(path.c_str(), true, texturePtr, false);
    if (!texturePtr) {
      _ERROR("Failed to load texture: {}", path);
      return nullptr;
    }
    auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(&*texturePtr);
    if (!newTexture) {
      _ERROR("Failed to cast texture to NiSourceTexture for map: {}", path);
      return nullptr;
    }
    return newTexture;
  }

  HRESULT ConvertTexture(ID3D11Device* device, ID3D11DeviceContext* context,
                         ID3D11Texture2D* srcTexture, DXGI_FORMAT newFormat,
                         ComPtr<ID3D11Texture2D>& outTexture) {
    if (!device || !context || !srcTexture) return false;

    D3D11_TEXTURE2D_DESC desc;
    srcTexture->GetDesc(&desc);
    DirectX::ScratchImage decoded;
    Lock();
    HRESULT hr = DirectX::CaptureTexture(device, context, srcTexture, decoded);
    Unlock();
    if (FAILED(hr)) {
      _ERROR("Failed to decode texture");
      return hr;
    }
    ComPtr<ID3D11Resource> newTexture;
    if (newFormat != DXGI_FORMAT_UNKNOWN && newFormat != desc.Format) {
      DirectX::ScratchImage converted;
      hr = DirectX::Decompress(decoded.GetImages(), decoded.GetImageCount(),
                               decoded.GetMetadata(), newFormat, converted);
      if (FAILED(hr)) {
        _TRACE("Failed to decompress texture, trying convert");
        hr = DirectX::Convert(decoded.GetImages(), decoded.GetImageCount(),
                              decoded.GetMetadata(), newFormat,
                              DirectX::TEX_FILTER_FANT, 0.0f, converted);
        if (FAILED(hr)) {
          _ERROR("Failed to convert texture format");
          return hr;
        }
      }
      hr = DirectX::CreateTexture(device, converted.GetImages(),
                                  converted.GetImageCount(),
                                  converted.GetMetadata(), &newTexture);
      if (FAILED(hr)) {
        _ERROR("Failed to create converted texture");
        return hr;
      }
    }
    outTexture.Reset();
    hr = newTexture.As(&outTexture);
    newTexture->Release();
    return hr;
  }

 private:
  // TODO: caching? Textures might be really large so it's up in the air
  struct TextureData {
    ComPtr<ID3D11Texture2D> texture;
    D3D11_TEXTURE2D_DESC texDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    struct MetaData {
      DXGI_FORMAT newFormat;
      UINT newWidth;
      UINT newHeight;
    };
    MetaData metaData;
  };

  class TextureLockGuard {
   public:
    TextureLockGuard() {
      EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
          &RE::BSGraphics::Renderer::GetSingleton()->GetLock()));
    }
    ~TextureLockGuard() {
      LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
          &RE::BSGraphics::Renderer::GetSingleton()->GetLock()));
    }

    TextureLockGuard(const TextureLockGuard&) = delete;
    TextureLockGuard& operator=(const TextureLockGuard&) = delete;
  };

  void Lock() {
    if (lockGuard_) {
      Unlock();
    }
    lockGuard_ = new TextureLockGuard();
  }
  void Unlock() {
    if (!lockGuard_) {
      return;
    }
    delete lockGuard_;
    lockGuard_ = nullptr;
  }

  TextureLockGuard* lockGuard_{nullptr};
};
}  // namespace Graphics