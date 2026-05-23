#pragma once

#include "Core/CriticalSectionLock.h"
#include "Filesystem.h"
#include "GeneratedShaderNames.h"
#include "Graphics/TextureLoader.h"
#include "Stats.h"

#include <unordered_map>

namespace Graphics {
class ShaderManager : public Singleton<ShaderManager> {
  using Blob = ComPtr<ID3DBlob>;
  using ComputeShader = ComPtr<ID3D11ComputeShader>;
  using PixelShader = ComPtr<ID3D11PixelShader>;
  using VertexShader = ComPtr<ID3D11VertexShader>;
  using Buffer = ComPtr<ID3D11Buffer>;
  using Device = ComPtr<ID3D11Device>;
  using Context = ComPtr<ID3D11DeviceContext>;
  using SamplerState = ComPtr<ID3D11SamplerState>;
  using ShaderResourceView = ComPtr<ID3D11ShaderResourceView>;
  using UnorderedAccessView = ComPtr<ID3D11UnorderedAccessView>;

  struct alignas(16) MaskedPaintBlendConstants {
    DirectX::XMFLOAT4 paintColor;
    UINT blendMode;
    UINT hasMask;
    float padding[2];
  };

  struct alignas(16) TextureChannelPaintBlendConstants {
    DirectX::XMFLOAT4 redChannelColor;
    DirectX::XMFLOAT4 greenChannelColor;
    DirectX::XMFLOAT4 blueChannelColor;
    UINT blendMode;
    float padding[3];
  };

  struct ShaderState {
    ComputeShader cs;
    ShaderResourceView srv[2]{};
    SamplerState ss;
    UnorderedAccessView uav;
    Buffer cb;
    ID3D11ClassInstance* ci[D3D11_SHADER_MAX_INTERFACES]{};
    UINT ciCount = D3D11_SHADER_MAX_INTERFACES;
  };

  struct PreparedPaintResources {
    ComPtr<ID3D11Texture2D> inputTexture;
    ComPtr<ID3D11ShaderResourceView> sourceSrv;
    ComPtr<ID3D11Texture2D> outputTexture;
    ComPtr<ID3D11UnorderedAccessView> outputUav;
    D3D11_TEXTURE2D_DESC inputDesc{};
    bool usedFallbackFormat{false};
  };

  static bool HasHlslExtension(string_view shaderName) {
    return shaderName.ends_with(".hlsl");
  }

 public:
  ShaderManager() = default;
  ~ShaderManager() = default;

  NOMOVE(ShaderManager);
  NOCOPY(ShaderManager);

  bool Initialize() {
    auto* device = (ID3D11Device*)RE::BSGraphics::Renderer::GetDevice();
    auto* ctx = (ID3D11DeviceContext*)
                    RE::BSGraphics::Renderer::GetRendererDataSingleton()
                        ->context;
    if (!device || !ctx) {
      _ERROR("Failed to acquire D3D11 device or context");
      return false;
    }
    device_ = device;
    context_ = ctx;
    return true;
  }

  ID3D11Device* GetDevice() const { return device_.Get(); }
  ID3D11DeviceContext* GetContext() const { return context_.Get(); }

  bool ApplyPaintBlendShader(ID3D11Texture2D* in,
                             ID3D11ShaderResourceView* inResourceView,
                             const char* maskPath,
                             DirectX::XMFLOAT4 paintColor, UINT blendMode,
                             string_view cacheKey,
                             ID3D11ShaderResourceView** out) {
    RETURN_IF_FALSE(in || inResourceView)
    RETURN_IF_FALSE(out)

    *out = nullptr;

    if ((!device_ || !context_) && !Initialize()) {
      return false;
    }
    ComputeShader shader;
    if (!GetComputeShader(ShaderNames::MaskedPaintBlendCS, shader)) {
      return false;
    }
    Buffer constantBuffer;
    if (!GetConstantBuffer(ShaderNames::MaskedPaintBlendCS,
                           sizeof(MaskedPaintBlendConstants),
                           constantBuffer)) {
      return false;
    }
    if (!EnsureSampler()) {
      return false;
    }
    if (TryLoadTextureFromCache(cacheKey, out)) {
      return true;
    }

    PreparedPaintResources prepared;
    if (!PreparePaintResources(in, inResourceView, prepared)) {
      return false;
    }

    ID3D11ShaderResourceView* maskSrv = nullptr;
    const bool hasMask = maskPath && maskPath[0] != '\0';
    if (hasMask) {
      auto* maskTexture = TextureLoader::LoadTexture(maskPath);
      RETURN_IF_FALSE(maskTexture)
      RETURN_IF_FALSE(maskTexture->rendererTexture)
      RETURN_IF_FALSE(maskTexture->rendererTexture->resourceView)
      maskSrv = maskTexture->rendererTexture->resourceView;
    }

    const MaskedPaintBlendConstants constants{.paintColor = paintColor,
                                              .blendMode = blendMode,
                                              .hasMask = hasMask ? 1u : 0u,
                                              .padding = {0.0f, 0.0f}};
    ID3D11ShaderResourceView* srvs[] = {
        prepared.sourceSrv.Get(),
        maskSrv,
    };
    ExecuteComputeShader(shader.Get(), constantBuffer.Get(), &constants,
                         srvs, prepared.outputUav.Get(), prepared.inputDesc.Width,
                         prepared.inputDesc.Height);

    if (!prepared.usedFallbackFormat) {
      WriteTextureToCache(prepared.outputTexture.Get(), cacheKey);
      return CreateOutputResourceView(prepared.outputTexture.Get(), out);
    }

    WriteTextureToCache(prepared.outputTexture.Get(), cacheKey);

    return CreateOutputResourceView(prepared.outputTexture.Get(), out);
  }

  bool ApplyTextureChannelPaintBlendShader(
      ID3D11Texture2D* in, ID3D11ShaderResourceView* inResourceView,
      const char* maskPath, DirectX::XMFLOAT4 redChannelColor,
      DirectX::XMFLOAT4 greenChannelColor,
      DirectX::XMFLOAT4 blueChannelColor, UINT blendMode,
      string_view cacheKey, ID3D11ShaderResourceView** out) {
    RETURN_IF_FALSE(in || inResourceView)
    RETURN_IF_FALSE(maskPath)
    RETURN_IF_FALSE(maskPath[0] != '\0')
    RETURN_IF_FALSE(out)

    *out = nullptr;

    if ((!device_ || !context_) && !Initialize()) {
      _ERROR("Device not initialized");
      return false;
    }
    ComputeShader shader;
    if (!GetComputeShader(ShaderNames::TextureChannelPaintBlendCS, shader)) {
      _ERROR("failed to get shader");
      return false;
    }
    Buffer constantBuffer;
    if (!GetConstantBuffer(ShaderNames::TextureChannelPaintBlendCS,
                           sizeof(TextureChannelPaintBlendConstants),
                           constantBuffer)) {
      _ERROR("Failed to get constant buffer for shader");
      return false;
    }
    if (!EnsureSampler()) {
      _ERROR("Sampler setup failed");
      return false;
    }
    if (TryLoadTextureFromCache(cacheKey, out)) {
      _DEBUG("Texture found in cache");
      return true;
    }

    PreparedPaintResources prepared;
    if (!PreparePaintResources(in, inResourceView, prepared)) {
      _ERROR("Failed to prepare paint resource");
      return false;
    }

    auto* maskTexture = TextureLoader::LoadTexture(maskPath);
    RETURN_IF_FALSE(maskTexture)
    RETURN_IF_FALSE(maskTexture->rendererTexture)
    RETURN_IF_FALSE(maskTexture->rendererTexture->resourceView)
    auto* maskSrv = maskTexture->rendererTexture->resourceView;

    const TextureChannelPaintBlendConstants constants{
        .redChannelColor = redChannelColor,
        .greenChannelColor = greenChannelColor,
        .blueChannelColor = blueChannelColor,
        .blendMode = blendMode,
        .padding = {0.0f, 0.0f, 0.0f}};
    ID3D11ShaderResourceView* srvs[] = {prepared.sourceSrv.Get(), maskSrv};
    ExecuteComputeShader(shader.Get(), constantBuffer.Get(),
                         &constants, srvs, prepared.outputUav.Get(),
                         prepared.inputDesc.Width, prepared.inputDesc.Height);

    if (!prepared.usedFallbackFormat) {
      WriteTextureToCache(prepared.outputTexture.Get(), cacheKey);
      return CreateOutputResourceView(prepared.outputTexture.Get(), out);
    }

    WriteTextureToCache(prepared.outputTexture.Get(), cacheKey);
    return CreateOutputResourceView(prepared.outputTexture.Get(), out);
  }

 private:
  bool PreparePaintResources(ID3D11Texture2D* in,
                             ID3D11ShaderResourceView* inResourceView,
                             PreparedPaintResources& out) const {
    RETURN_IF_FALSE(in || inResourceView)

    if (in) {
      out.inputTexture = in;
    } else {
      ComPtr<ID3D11Resource> inputResource;
      inResourceView->GetResource(&inputResource);
      if (!inputResource || FAILED(inputResource.As(&out.inputTexture))) {
        _ERROR("Failed to recover source texture from shader resource view");
        return false;
      }
    }

    out.inputTexture->GetDesc(&out.inputDesc);
    if (out.inputDesc.ArraySize != 1 || out.inputDesc.SampleDesc.Count != 1) {
      _ERROR("Paint blend shader only supports non-array, non-MSAA textures");
      return false;
    }

    auto* textureLoader = TextureLoader::GetSingleton();
    ComPtr<ID3D11Texture2D> workingInputTexture = out.inputTexture;
    out.sourceSrv = inResourceView;

    if (!TryCreatePaintResources(out.inputTexture.Get(), out.inputDesc.Format,
                                 inResourceView, out.sourceSrv,
                                 out.outputTexture, out.outputUav)) {
      _TRACE("Falling back to paintable intermediate format for texture format {}",
             static_cast<uint32_t>(out.inputDesc.Format));

      const HRESULT convertHr = textureLoader->ConvertTexture(
          device_.Get(), context_.Get(), out.inputTexture.Get(),
          DXGI_FORMAT_R8G8B8A8_UNORM, workingInputTexture);
      if (FAILED(convertHr)) {
        _ERROR("Failed to convert input texture to paintable format: {:08X}",
               convertHr);
        return false;
      }

      if (!TryCreatePaintResources(workingInputTexture.Get(),
                                   DXGI_FORMAT_R8G8B8A8_UNORM, nullptr,
                                   out.sourceSrv, out.outputTexture,
                                   out.outputUav)) {
        _ERROR("Failed to create paint resources for fallback format");
        return false;
      }

      out.usedFallbackFormat = true;
    }

    return true;
  }

  bool GetShaderBlob(string_view shaderName, string_view entryPoint,
                     string_view target, Blob& out) {
    RETURN_IF_FALSE(!HasHlslExtension(shaderName))

    const string shaderKey(shaderName);
    if (const auto it = shaderBlobs_.find(shaderKey); it != shaderBlobs_.end()) {
      out = it->second;
      return true;
    }

    const auto shaderSourcePath =
        Filesystem::Join(Filesystem::MATERIAL_SHADER_DIR,
                         string(shaderName) + ".hlsl");
    const auto compiledShaderDir =
        Filesystem::Join(Filesystem::MATERIAL_SHADER_DIR, "compiled");
    const auto compiledShaderPath = Filesystem::Join(
        compiledShaderDir, string(shaderName) + "." + string(target) + ".cso");

    error_code ec;
    fs::create_directories(compiledShaderDir, ec);
    if (ec) {
      _WARN("Failed to create shader cache directory {}: {}",
            compiledShaderDir.string(), ec.message());
    }

    if (fs::exists(compiledShaderPath, ec) && !ec) {
      bool shouldCompile = true;
      if (error_code sourceEc; fs::exists(shaderSourcePath, sourceEc) && !sourceEc) {
        const auto compiledTime = fs::last_write_time(compiledShaderPath, ec);
        const auto sourceTime = fs::last_write_time(shaderSourcePath, sourceEc);
        shouldCompile = ec || sourceEc || compiledTime < sourceTime;
      }
      if (!shouldCompile) {
        const HRESULT readHr = D3DReadFileToBlob(compiledShaderPath.wstring().c_str(),
                                                out.GetAddressOf());
        if (SUCCEEDED(readHr)) {
          shaderBlobs_[shaderKey] = out;
          return true;
        }
        _WARN("Failed to read compiled shader {}, recompiling", compiledShaderPath.string());
        out.Reset();
      }
    }

    Blob errorBlob;
    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifndef NDEBUG
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    const HRESULT compileHr = D3DCompileFromFile(
        shaderSourcePath.wstring().c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint.data(), target.data(), compileFlags, 0, out.GetAddressOf(),
        errorBlob.GetAddressOf());
    if (FAILED(compileHr)) {
      _ERROR("Failed to compile shader {}: {}", shaderSourcePath.string(),
             errorBlob ? static_cast<const char*>(errorBlob->GetBufferPointer())
                       : "unknown error");
      return false;
    }

    const HRESULT writeHr =
        D3DWriteBlobToFile(out.Get(), compiledShaderPath.wstring().c_str(), TRUE);
    if (FAILED(writeHr)) {
      _WARN("Failed to write compiled shader {}: {:08X}",
            compiledShaderPath.string(), writeHr);
    }

    shaderBlobs_[shaderKey] = out;
    return true;
  }

  bool GetComputeShader(string_view shaderName, ComputeShader& out) {
    RETURN_IF_FALSE(!HasHlslExtension(shaderName))

    const string shaderKey(shaderName);
    if (const auto it = computeShaders_.find(shaderKey); it != computeShaders_.end()) {
      out = it->second;
      return true;
    }

    Blob shaderBlob;
    if (!GetShaderBlob(shaderName, "MainCS", "cs_5_0", shaderBlob)) {
      return false;
    }

    ComputeShader shader;
    const HRESULT shaderHr = device_->CreateComputeShader(
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
        &shader);
    if (FAILED(shaderHr)) {
      _ERROR("Failed to create compute shader {}: {:08X}", string(shaderName),
             shaderHr);
      return false;
    }

    computeShaders_[shaderKey] = shader;
    out = shader;
    return true;
  }

  bool GetPixelShader(string_view shaderName, PixelShader& out) {
    UNUSED(shaderName);
    UNUSED(out);
    NOT_IMPLEMENTED
  }

  bool GetVertexShader(string_view shaderName, VertexShader& out) {
    UNUSED(shaderName);
    UNUSED(out);
    NOT_IMPLEMENTED
  }

  bool GetConstantBuffer(string_view shaderName, UINT byteWidth, Buffer& out) {
    const string shaderKey(shaderName);
    if (const auto it = constantBuffers_.find(shaderKey); it != constantBuffers_.end()) {
      out = it->second;
      return true;
    }

    D3D11_BUFFER_DESC constantBufferDesc{};
    constantBufferDesc.ByteWidth = byteWidth;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    Buffer buffer;
    const HRESULT bufferHr =
        device_->CreateBuffer(&constantBufferDesc, nullptr, &buffer);
    if (FAILED(bufferHr)) {
      _ERROR("Failed to create shader constant buffer {}: {:08X}", shaderKey,
             bufferHr);
      return false;
    }

    constantBuffers_[shaderKey] = buffer;
    out = buffer;
    return true;
  }

  bool EnsureSampler() {
    if (linearClampSampler_) {
      return true;
    }

    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    const HRESULT samplerHr =
        device_->CreateSamplerState(&samplerDesc, &linearClampSampler_);
    if (FAILED(samplerHr)) {
      _ERROR("Failed to create paint blend sampler: {:08X}", samplerHr);
      return false;
    }

    return true;
  }

  void ExecuteComputeShader(ID3D11ComputeShader* shader, ID3D11Buffer* constantBuffer,
                            const void* constants,
                            ID3D11ShaderResourceView* const* srvs,
                            ID3D11UnorderedAccessView* outputUav, UINT width,
                            UINT height) const {
    context_->UpdateSubresource(constantBuffer, 0, nullptr, constants, 0, 0);
    ShaderState old;
    auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
    CriticalSectionLock lock(
        reinterpret_cast<LPCRITICAL_SECTION>(&renderer->GetLock()));

    ID3D11ComputeShader* previousShaderRaw = nullptr;
    context_->CSGetShader(&previousShaderRaw, old.ci,
                          &old.ciCount);
    old.cs.Attach(previousShaderRaw);

    ID3D11ShaderResourceView* previousSrvsRaw[2]{};
    context_->CSGetShaderResources(0, size(previousSrvsRaw), previousSrvsRaw);
    for (size_t i = 0; i < size(previousSrvsRaw); ++i) {
      old.srv[i].Attach(previousSrvsRaw[i]);
    }

    ID3D11UnorderedAccessView* previousUavRaw = nullptr;
    context_->CSGetUnorderedAccessViews(0, 1, &previousUavRaw);
    old.uav.Attach(previousUavRaw);

    ID3D11Buffer* previousConstantBufferRaw = nullptr;
    context_->CSGetConstantBuffers(0, 1, &previousConstantBufferRaw);
    old.cb.Attach(previousConstantBufferRaw);

    ID3D11SamplerState* previousSamplerRaw = nullptr;
    context_->CSGetSamplers(0, 1, &previousSamplerRaw);
    old.ss.Attach(previousSamplerRaw);

    ID3D11UnorderedAccessView* uavs[] = {outputUav};
    ID3D11Buffer* constantBuffers[] = {constantBuffer};
    ID3D11SamplerState* samplers[] = {linearClampSampler_.Get()};
    constexpr UINT uavCounts[] = {0};

    context_->CSSetShader(shader, nullptr, 0);
    context_->CSSetShaderResources(0, 2, srvs);
    context_->CSSetUnorderedAccessViews(0, 1, uavs, uavCounts);
    context_->CSSetConstantBuffers(0, 1, constantBuffers);
    context_->CSSetSamplers(0, 1, samplers);
    context_->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

    ID3D11ShaderResourceView* nullSrvs[2]{};
    ID3D11UnorderedAccessView* nullUavs[1]{};
    context_->CSSetShaderResources(0, 2, nullSrvs);
    context_->CSSetUnorderedAccessViews(0, 1, nullUavs, uavCounts);

    ID3D11ShaderResourceView* restoreSrvs[] = {old.srv[0].Get(), old.srv[1].Get()};
    ID3D11UnorderedAccessView* restoreUavs[] = {old.uav.Get()};
    ID3D11Buffer* restoreConstantBuffers[] = {old.cb.Get()};
    ID3D11SamplerState* restoreSamplers[] = {old.ss.Get()};
    constexpr UINT restoreUavCounts[] = {D3D11_KEEP_UNORDERED_ACCESS_VIEWS};

    context_->CSSetShader(old.cs.Get(), old.ci,
                          old.ciCount);
    context_->CSSetShaderResources(0, 2, restoreSrvs);
    context_->CSSetUnorderedAccessViews(0, 1, restoreUavs, restoreUavCounts);
    context_->CSSetConstantBuffers(0, 1, restoreConstantBuffers);
    context_->CSSetSamplers(0, 1, restoreSamplers);

    for (UINT i = 0; i < old.ciCount; ++i) {
      RELEASE(old.ci[i]);
    }
  }

  bool CreateOutputResourceView(ID3D11Texture2D* texture,
                                ID3D11ShaderResourceView** out) const {
    RETURN_IF_FALSE(texture)
    RETURN_IF_FALSE(out)

    D3D11_TEXTURE2D_DESC textureDesc{};
    texture->GetDesc(&textureDesc);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    const HRESULT hr =
        device_->CreateShaderResourceView(texture, &srvDesc, out);
    if (FAILED(hr)) {
      _ERROR("Failed to create output SRV: {:08X}", hr);
      return false;
    }

    return true;
  }

  bool TryLoadTextureFromCache(string_view cacheKey,
                               ID3D11ShaderResourceView** out) const {
    if (cacheKey.empty()) {
      return false;
    }

    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage cachedTexture;
    const auto cachePath = Filesystem::Join(Filesystem::MATERIAL_CACHE_DIR,
                                            string(cacheKey) + ".dds");
    const HRESULT loadHr = DirectX::LoadFromDDSFile(
        cachePath.wstring().c_str(), DirectX::DDS_FLAGS_NONE, &metadata,
        cachedTexture);
    if (FAILED(loadHr)) {
      return false;
    }

    ComPtr<ID3D11Resource> resource;
    const HRESULT createHr = DirectX::CreateTexture(
        device_.Get(), cachedTexture.GetImages(), cachedTexture.GetImageCount(),
        cachedTexture.GetMetadata(), &resource);
    if (FAILED(createHr)) {
      _WARN("Failed to create cached texture {}: {:08X}", cachePath.string(),
            createHr);
      return false;
    }

    ComPtr<ID3D11Texture2D> texture;
    if (FAILED(resource.As(&texture))) {
      _WARN("Failed to cast cached texture {} to ID3D11Texture2D", cachePath.string());
      return false;
    }

    if (!CreateOutputResourceView(texture.Get(), out)) {
      return false;
    }

    _TRACE("Loaded cached texture {}", cachePath.string());
    return true;
  }

  void WriteTextureToCache(ID3D11Texture2D* texture, string_view cacheKey) const {
    if (cacheKey.empty()) {
      return;
    }
    if (!texture) {
      return;
    }

    error_code ec;
    fs::create_directories(Filesystem::MATERIAL_CACHE_DIR, ec);
    if (ec) {
      _WARN("Failed to create cache directory {}: {}",
            Filesystem::MATERIAL_CACHE_DIR, ec.message());
      return;
    }

    DirectX::ScratchImage capturedTexture;
    {
      auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
      CriticalSectionLock lock(
          reinterpret_cast<LPCRITICAL_SECTION>(&renderer->GetLock()));
      const HRESULT captureHr =
          DirectX::CaptureTexture(device_.Get(), context_.Get(), texture,
                                  capturedTexture);
      if (FAILED(captureHr)) {
        _WARN("Failed to capture cached texture {}: {:08X}", cacheKey,
              captureHr);
        return;
      }
    }

    const auto cachePath = Filesystem::Join(Filesystem::MATERIAL_CACHE_DIR,
                                            string(cacheKey) + ".dds");
    const HRESULT saveHr = DirectX::SaveToDDSFile(
        capturedTexture.GetImages(), capturedTexture.GetImageCount(),
        capturedTexture.GetMetadata(), DirectX::DDS_FLAGS_NONE,
        cachePath.wstring().c_str());
    if (FAILED(saveHr)) {
      _WARN("Failed to write cached texture {}: {:08X}", cachePath.string(),
            saveHr);
      return;
    }

    _TRACE("Wrote cached texture {}", cachePath.string());
  }

  bool TryCreatePaintResources(ID3D11Texture2D* sourceTexture,
                               DXGI_FORMAT resourceFormat,
                               ID3D11ShaderResourceView* preferredSourceSrv,
                               ComPtr<ID3D11ShaderResourceView>& sourceSrv,
                               ComPtr<ID3D11Texture2D>& outputTexture,
                               ComPtr<ID3D11UnorderedAccessView>& outputUav) const {
    RETURN_IF_FALSE(sourceTexture)

    sourceSrv.Reset();
    outputTexture.Reset();
    outputUav.Reset();

    if (preferredSourceSrv) {
      sourceSrv = preferredSourceSrv;
    } else {
      D3D11_SHADER_RESOURCE_VIEW_DESC sourceSrvDesc{};
      sourceSrvDesc.Format = resourceFormat;
      sourceSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      sourceSrvDesc.Texture2D.MostDetailedMip = 0;
      sourceSrvDesc.Texture2D.MipLevels = 1;

      const HRESULT srvHr = device_->CreateShaderResourceView(
          sourceTexture, &sourceSrvDesc, &sourceSrv);
      if (FAILED(srvHr)) {
        _TRACE("Failed to create source SRV for format {}: {:08X}",
               static_cast<uint32_t>(resourceFormat), srvHr);
        return false;
      }
    }

    D3D11_TEXTURE2D_DESC sourceDesc{};
    sourceTexture->GetDesc(&sourceDesc);

    D3D11_TEXTURE2D_DESC outputDesc{};
    outputDesc.Width = sourceDesc.Width;
    outputDesc.Height = sourceDesc.Height;
    outputDesc.MipLevels = 1;
    outputDesc.ArraySize = 1;
    outputDesc.Format = resourceFormat;
    outputDesc.SampleDesc.Count = 1;
    outputDesc.SampleDesc.Quality = 0;
    outputDesc.Usage = D3D11_USAGE_DEFAULT;
    outputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

    const HRESULT textureHr =
        device_->CreateTexture2D(&outputDesc, nullptr, &outputTexture);
    if (FAILED(textureHr)) {
      _TRACE("Failed to create output texture for format {}: {:08X}",
             static_cast<uint32_t>(resourceFormat), textureHr);
      return false;
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC outputUavDesc{};
    outputUavDesc.Format = resourceFormat;
    outputUavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    outputUavDesc.Texture2D.MipSlice = 0;

    const HRESULT uavHr = device_->CreateUnorderedAccessView(
        outputTexture.Get(), &outputUavDesc, &outputUav);
    if (FAILED(uavHr)) {
      _TRACE("Failed to create output UAV for format {}: {:08X}",
             static_cast<uint32_t>(resourceFormat), uavHr);
      return false;
    }

    return true;
  }

  Device device_;
  Context context_;
  SamplerState linearClampSampler_;
  unordered_map<string, Blob> shaderBlobs_;
  unordered_map<string, ComputeShader> computeShaders_;
  unordered_map<string, PixelShader> pixelShaders_;
  unordered_map<string, VertexShader> vertexShaders_;
  unordered_map<string, Buffer> constantBuffers_;
};
}  // namespace Graphics
