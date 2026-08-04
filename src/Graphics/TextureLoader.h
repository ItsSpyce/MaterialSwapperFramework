#pragma once

#include "RE/Misc.h"

namespace Graphics {

class TextureLoader : public Singleton<TextureLoader> {
 public:
  TextureLoader() {}

  static RE::NiPointer<RE::NiSourceTexture> LoadTexture(const string& path) {
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
    return RE::NiPointer(newTexture);
  }

  HRESULT ConvertTexture(ID3D11Device* device, ID3D11DeviceContext* context,
                         ID3D11Texture2D* srcTexture, DXGI_FORMAT newFormat,
                         ComPtr<ID3D11Texture2D>& outTexture) {
    if (!device || !context || !srcTexture) {
      return E_INVALIDARG;
    }

    D3D11_TEXTURE2D_DESC desc{};
    srcTexture->GetDesc(&desc);
    DirectX::ScratchImage decoded;
    Lock();
    HRESULT hr = DirectX::CaptureTexture(device, context, srcTexture, decoded);
    _TRACE("Texture captured");
    Unlock();
    if (FAILED(hr)) {
      _ERROR("Failed to decode texture");
      return hr;
    }

    const DXGI_FORMAT targetFormat =
        newFormat == DXGI_FORMAT_UNKNOWN ? desc.Format : newFormat;

    const DirectX::Image* outputImages = decoded.GetImages();
    size_t outputImageCount = decoded.GetImageCount();
    auto outputMetadata = decoded.GetMetadata();
    DirectX::ScratchImage linearImage;
    DirectX::ScratchImage convertedImage;
    DirectX::ScratchImage compressedImage;

    if (targetFormat != outputMetadata.format) {
      if (DirectX::IsCompressed(targetFormat)) {
        const DirectX::Image* compressImages = outputImages;
        size_t compressImageCount = outputImageCount;
        auto compressMetadata = outputMetadata;
        auto compressFlags = DirectX::TEX_COMPRESS_DEFAULT;

        if (targetFormat == DXGI_FORMAT_BC7_UNORM ||
            targetFormat == DXGI_FORMAT_BC7_UNORM_SRGB) {
          compressFlags |= DirectX::TEX_COMPRESS_BC7_QUICK;
        }

        compressFlags |= DirectX::TEX_COMPRESS_PARALLEL;

        if (DirectX::IsCompressed(outputMetadata.format)) {
          hr = DirectX::Decompress(decoded.GetImages(), decoded.GetImageCount(),
                                   outputMetadata, DXGI_FORMAT_R8G8B8A8_UNORM,
                                   linearImage);
          _TRACE("Decompressed decoded images 1");
          if (FAILED(hr)) {
            _ERROR("Failed to decompress texture for compression");
            return hr;
          }
          compressImages = linearImage.GetImages();
          compressImageCount = linearImage.GetImageCount();
          compressMetadata = linearImage.GetMetadata();
        } else {
          _TRACE("Texture uncompressed");
        }

        _TRACE("Compressing texture to target format {}", static_cast<uint32_t>(targetFormat));
        hr = DirectX::Compress(compressImages, compressImageCount,
                               compressMetadata, targetFormat,
                               compressFlags, 1.0f,
                               compressedImage);
        _TRACE("Compressed input image");
        if (FAILED(hr)) {
          _ERROR("Failed to compress texture format");
          return hr;
        }

        outputImages = compressedImage.GetImages();
        outputImageCount = compressedImage.GetImageCount();
        outputMetadata = compressedImage.GetMetadata();
      } else {
        hr = DirectX::Decompress(decoded.GetImages(), decoded.GetImageCount(),
                                 outputMetadata, targetFormat, convertedImage);
        _TRACE("Decompressed decoded images 2");
        if (FAILED(hr)) {
          _TRACE("Failed to decompress texture, trying convert");
          hr = DirectX::Convert(decoded.GetImages(), decoded.GetImageCount(),
                                outputMetadata, targetFormat,
                                DirectX::TEX_FILTER_FANT, 0.0f,
                                convertedImage);
          if (FAILED(hr)) {
            _ERROR("Failed to convert texture format");
            return hr;
          }
        }

        outputImages = convertedImage.GetImages();
        outputImageCount = convertedImage.GetImageCount();
        outputMetadata = convertedImage.GetMetadata();
      }
    }

    outTexture.Reset();
    if (!outputImages || outputImageCount == 0) {
      return E_FAIL;
    }

    const auto& topImage = outputImages[0];

    D3D11_TEXTURE2D_DESC outputDesc{};
    outputDesc.Width = static_cast<UINT>(outputMetadata.width);
    outputDesc.Height = static_cast<UINT>(outputMetadata.height);
    outputDesc.MipLevels = 1;
    outputDesc.ArraySize = 1;
    outputDesc.Format = outputMetadata.format;
    outputDesc.SampleDesc.Count = 1;
    outputDesc.SampleDesc.Quality = 0;
    outputDesc.Usage = D3D11_USAGE_DEFAULT;
    outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    hr = device->CreateTexture2D(&outputDesc, nullptr, &outTexture);
    if (FAILED(hr)) {
      _ERROR("Failed to create converted texture");
      return hr;
    }

    Lock();
    context->UpdateSubresource(outTexture.Get(), 0, nullptr, topImage.pixels,
                               static_cast<UINT>(topImage.rowPitch),
                               static_cast<UINT>(topImage.slicePitch));
    Unlock();
    _TRACE("Created and uploaded new texture");

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
