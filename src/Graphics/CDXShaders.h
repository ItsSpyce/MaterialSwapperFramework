#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

class CDXD3DDevice;
class CDXShaderFile;
class CDXShaderFactory;
class CDXShader;
class CDXMaterial;
class CDXCamera;

typedef DirectX::XMMATRIX CDXMatrix;
typedef unsigned short CDXMeshIndex;
typedef DirectX::XMVECTOR CDXVec;
typedef DirectX::XMFLOAT4 CDXVec4;
typedef DirectX::XMFLOAT3 CDXVec3;
typedef DirectX::XMFLOAT2 CDXVec2;
typedef DirectX::XMFLOAT3 CDXColor;

typedef std::set<CDXMeshIndex> CDXMeshIndexSet;
typedef std::unordered_map<CDXMeshIndex, float> CDXHitIndexMap;

struct CDXInitParams {
  CDXD3DDevice* device;
  CDXCamera* camera;
  CDXShaderFile* vShader[2];
  CDXShaderFile* pShader[2];
  CDXShaderFactory* factory;
  int viewportWidth;
  int viewportHeight;
};
struct CDXMeshEdge {
  CDXMeshIndex p1;
  CDXMeshIndex p2;

  CDXMeshEdge(CDXMeshIndex _p1, CDXMeshIndex _p2) {
    p1 = _p1;
    p2 = _p2;
  }
};

struct CDXMeshFace {
  CDXMeshIndex v1;
  CDXMeshIndex v2;
  CDXMeshIndex v3;

  CDXMeshFace(CDXMeshIndex _v1, CDXMeshIndex _v2, CDXMeshIndex _v3) {
    v1 = _v1;
    v2 = _v2;
    v3 = _v3;
  }
};

namespace std {
template <>
struct hash<CDXMeshEdge> {
  std::size_t operator()(const CDXMeshEdge& t) const noexcept {
    return t.p2 << 16 | t.p1 & 0xFFFF;
  }
};

template <>
struct equal_to<CDXMeshEdge> {
  bool operator()(const CDXMeshEdge& t1, const CDXMeshEdge& t2) const {
    return t1.p1 == t2.p1 && t1.p2 == t2.p2;
  }
};

template <>
struct hash<CDXMeshFace> {
  std::size_t operator()(const CDXMeshFace& t) const noexcept {
    char* d = (char*)&t;
    size_t len = sizeof(CDXMeshFace);
    size_t hash, i;
    for (hash = i = 0; i < len; ++i) {
      hash += d[i];
      hash += hash << 10;
      hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
  }
};
template <>
struct equal_to<CDXMeshFace> {
  bool operator()(const CDXMeshFace& t1, const CDXMeshFace& t2) const {
    return t1.v1 == t2.v1 && t1.v2 == t2.v2 && t1.v3 == t2.v3;
  }
};
}  // namespace std

struct CDXMeshVert {
  CDXVec3 Position;
  CDXVec2 Tex;
  CDXVec3 Normal;
  CDXColor Color;
};

class CDXPixelShaderCache
    : protected unordered_map<string, ComPtr<ID3D11PixelShader>> {
 public:
  explicit CDXPixelShaderCache(CDXShaderFactory* factory) : factory_(factory) {}

  virtual ComPtr<ID3D11PixelShader> GetShader(CDXD3DDevice* device,
                                              const string& name) {
    if (auto it = find(name); it != end()) {
      return it->second;
    }
    return nullptr;
  }

 protected:
  CDXShaderFactory* factory_;
};

typedef HRESULT (*_D3DCompile)(LPCVOID pSrcData, SIZE_T SrcDataSize,
                               LPCSTR pSourceName,
                               const D3D_SHADER_MACRO* pDefines,
                               ID3DInclude* pInclude, LPCSTR pEntrypoint,
                               LPCSTR pTarget, UINT Flags1, UINT Flags2,
                               ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);

inline HRESULT CompileShaderFromData(LPCVOID srcData, _In_ SIZE_T srcDataSize,
                                     _In_opt_ LPCSTR sourceName,
                                     _In_ LPCSTR entryPoint,
                                     _In_ LPCSTR profile,
                                     _Outptr_ ID3DBlob** blob,
                                     _Outptr_ ID3DBlob** errorBlob) {
  if (!srcData || !entryPoint || !profile || !blob) {
    return E_INVALIDARG;
  }
  *blob = nullptr;
  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG;
#else
  flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

  constexpr D3D_SHADER_MACRO defines[] = {NULL, NULL};
  char name[MAX_PATH];
  static constexpr auto VERSIONS = {"47", "46e", "45", "44", "43", "42"};

  HMODULE d3dcompiler = 0;
  for (auto& version : VERSIONS) {
    _snprintf_s(name, MAX_PATH, "d3dcompiler_%s.dll", version);
    d3dcompiler = LoadLibraryA(name);
    if (d3dcompiler) {
      break;
    }
  }
  if (!d3dcompiler) {
    _ERROR("Failed to load d3dcompiler_xx.dll");
    return E_NOINTERFACE;
  }
  auto D3DCompile = (_D3DCompile)GetProcAddress(d3dcompiler, "D3DCompile");
  if (!D3DCompile) {
    _ERROR("Failed to get D3DCompile address");
    FreeLibrary(d3dcompiler);
    return E_NOINTERFACE;
  }
  ID3DBlob* shaderBlob = nullptr;
  HRESULT hr =
      D3DCompile(srcData, srcDataSize, sourceName, defines, nullptr, entryPoint,
                 profile, flags, 0, &shaderBlob, errorBlob);
  if (FAILED(hr)) {
    if (shaderBlob) {
      shaderBlob->Release();
    }
    FreeLibrary(d3dcompiler);
    return hr;
  }
  *blob = shaderBlob;
  return hr;
}

inline HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint,
                             _In_ LPCSTR profile, _Outptr_ ID3DBlob** code,
                             _Outptr_opt_ ID3DBlob** errorMsgs) {
  if (!srcFile || !entryPoint || !profile || !code) {
    return E_INVALIDARG;
  }

  *code = nullptr;
  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG;
#else
  flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

  const D3D_SHADER_MACRO defines[] = {NULL, NULL};

  ID3DBlob* shaderBlob = nullptr;
  HRESULT hr =
      D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                         entryPoint, profile, flags, 0, &shaderBlob, errorMsgs);
  if (FAILED(hr)) {
    if (shaderBlob) {
      shaderBlob->Release();
    }
    return hr;
  }
  *code = shaderBlob;
  return hr;
}

class CDXD3DDevice {
 public:
  CDXD3DDevice() : device_(nullptr), deviceContext_(nullptr) {}
  CDXD3DDevice(const ComPtr<ID3D11Device>& device,
               const ComPtr<ID3D11DeviceContext>& deviceContext)
      : device_(device), deviceContext_(deviceContext) {}

  void SetDevice(const ComPtr<ID3D11Device>& device) { device_ = device; }
  void SetDeviceContext(const ComPtr<ID3D11DeviceContext>& deviceContext) {
    deviceContext_ = deviceContext;
  }

  ComPtr<ID3D11Device>& GetDevice() { return device_; }
  ComPtr<ID3D11DeviceContext>& GetDeviceContext() { return deviceContext_; }

 protected:
  ComPtr<ID3D11Device> device_;
  ComPtr<ID3D11DeviceContext> deviceContext_;
};

class CDXRenderState {
 public:
  void BackupRenderState(CDXD3DDevice* device) {
    auto ctx = device->GetDeviceContext();

    ctx->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
                            backupState_.RenderTargetViews,
                            &backupState_.DepthStencilView);

    backupState_.ScissorRectsCount = backupState_.ViewportsCount =
        D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    ctx->RSGetScissorRects(&backupState_.ScissorRectsCount,
                           backupState_.ScissorRects);
    ctx->RSGetViewports(&backupState_.ViewportsCount, backupState_.Viewports);
    ctx->RSGetState(&backupState_.RS);
    ctx->OMGetBlendState(&backupState_.BlendState, backupState_.BlendFactor,
                         &backupState_.SampleMask);
    ctx->OMGetDepthStencilState(&backupState_.DepthStencilState,
                                &backupState_.StencilRef);
    ctx->PSGetShaderResources(0, 1, &backupState_.PSShaderResource);
    ctx->PSGetSamplers(0, 1, &backupState_.PSSampler);
    ctx->GSGetShaderResources(0, 1, &backupState_.GSShaderResource);
    backupState_.PSInstancesCount = backupState_.VSInstancesCount =
        backupState_.GSInstancesCount = 256;
    ctx->PSGetShader(&backupState_.PS, backupState_.PSInstances,
                     &backupState_.PSInstancesCount);
    ctx->VSGetShader(&backupState_.VS, backupState_.VSInstances,
                     &backupState_.VSInstancesCount);
    ctx->GSGetShader(&backupState_.GS, backupState_.GSInstances,
                     &backupState_.GSInstancesCount);
    ctx->GSGetConstantBuffers(0, 1, &backupState_.GSConstantBuffer);
    ctx->VSGetConstantBuffers(0, 1, &backupState_.VSConstantBuffer);
    ctx->IAGetPrimitiveTopology(&backupState_.PrimitiveTopology);
    ctx->IAGetIndexBuffer(&backupState_.IndexBuffer,
                          &backupState_.IndexBufferFormat,
                          &backupState_.IndexBufferOffset);
    ctx->IAGetVertexBuffers(0, 1, &backupState_.VertexBuffer,
                            &backupState_.VertexBufferStride,
                            &backupState_.VertexBufferOffset);
    ctx->IAGetInputLayout(&backupState_.InputLayout);
  }
  void RestoreRenderState(CDXD3DDevice* device) const {
    auto ctx = device->GetDeviceContext();

    ctx->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
                            backupState_.RenderTargetViews,
                            backupState_.DepthStencilView);
    for (u32 i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
      if (backupState_.RenderTargetViews[i]) {
        backupState_.RenderTargetViews[i]->Release();
      }
    }
    if (backupState_.DepthStencilView) backupState_.DepthStencilView->Release();

    // Restore modified DX state
    ctx->RSSetScissorRects(backupState_.ScissorRectsCount,
                           backupState_.ScissorRects);
    ctx->RSSetViewports(backupState_.ViewportsCount, backupState_.Viewports);
    ctx->RSSetState(backupState_.RS);
    if (backupState_.RS) backupState_.RS->Release();
    ctx->OMSetBlendState(backupState_.BlendState, backupState_.BlendFactor,
                         backupState_.SampleMask);
    if (backupState_.BlendState) backupState_.BlendState->Release();
    ctx->OMSetDepthStencilState(backupState_.DepthStencilState,
                                backupState_.StencilRef);
    if (backupState_.DepthStencilState)
      backupState_.DepthStencilState->Release();
    ctx->PSSetShaderResources(0, 1, &backupState_.PSShaderResource);
    if (backupState_.PSShaderResource) backupState_.PSShaderResource->Release();
    ctx->PSSetSamplers(0, 1, &backupState_.PSSampler);
    if (backupState_.PSSampler) backupState_.PSSampler->Release();
    ctx->PSSetShader(backupState_.PS, backupState_.PSInstances,
                     backupState_.PSInstancesCount);
    if (backupState_.PS) backupState_.PS->Release();
    for (UINT i = 0; i < backupState_.PSInstancesCount; i++)
      if (backupState_.PSInstances[i]) backupState_.PSInstances[i]->Release();
    ctx->VSSetShader(backupState_.VS, backupState_.VSInstances,
                     backupState_.VSInstancesCount);
    if (backupState_.VS) backupState_.VS->Release();
    ctx->VSSetConstantBuffers(0, 1, &backupState_.VSConstantBuffer);
    if (backupState_.VSConstantBuffer) backupState_.VSConstantBuffer->Release();
    for (UINT i = 0; i < backupState_.VSInstancesCount; i++)
      if (backupState_.VSInstances[i]) backupState_.VSInstances[i]->Release();
    ctx->GSSetShader(backupState_.GS, backupState_.GSInstances,
                     backupState_.GSInstancesCount);
    if (backupState_.GS) backupState_.GS->Release();
    ctx->GSSetShaderResources(0, 1, &backupState_.GSShaderResource);
    if (backupState_.GSShaderResource) backupState_.GSShaderResource->Release();
    ctx->GSSetConstantBuffers(0, 1, &backupState_.GSConstantBuffer);
    if (backupState_.GSConstantBuffer) backupState_.GSConstantBuffer->Release();
    for (UINT i = 0; i < backupState_.GSInstancesCount; i++)
      if (backupState_.GSInstances[i]) backupState_.GSInstances[i]->Release();
    ctx->IASetPrimitiveTopology(backupState_.PrimitiveTopology);
    ctx->IASetIndexBuffer(backupState_.IndexBuffer,
                          backupState_.IndexBufferFormat,
                          backupState_.IndexBufferOffset);
    if (backupState_.IndexBuffer) backupState_.IndexBuffer->Release();
    ctx->IASetVertexBuffers(0, 1, &backupState_.VertexBuffer,
                            &backupState_.VertexBufferStride,
                            &backupState_.VertexBufferOffset);
    if (backupState_.VertexBuffer) backupState_.VertexBuffer->Release();
    ctx->IASetInputLayout(backupState_.InputLayout);
    if (backupState_.InputLayout) backupState_.InputLayout->Release();
  }

 private:
  struct BACKUP_DX11_STATE {
    UINT ScissorRectsCount, ViewportsCount;
    D3D11_RECT
    ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    D3D11_VIEWPORT
    Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    ID3D11RasterizerState* RS;
    ID3D11BlendState* BlendState;
    FLOAT BlendFactor[4];
    UINT SampleMask;
    UINT StencilRef;
    ID3D11DepthStencilState* DepthStencilState;
    ID3D11ShaderResourceView* PSShaderResource;
    ID3D11SamplerState* PSSampler;
    ID3D11ShaderResourceView* GSShaderResource;
    ID3D11PixelShader* PS;
    ID3D11VertexShader* VS;
    ID3D11GeometryShader* GS;
    UINT PSInstancesCount, VSInstancesCount, GSInstancesCount;
    ID3D11ClassInstance *PSInstances[256], *VSInstances[256],
        *GSInstances[256];  // 256 is max according to PSSetShader documentation
    D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;
    ID3D11Buffer *IndexBuffer, *VertexBuffer, *VSConstantBuffer,
        *GSConstantBuffer;
    UINT IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
    DXGI_FORMAT IndexBufferFormat;
    ID3D11InputLayout* InputLayout;
    ID3D11RenderTargetView*
        RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    ID3D11DepthStencilView* DepthStencilView;
  };

  BACKUP_DX11_STATE backupState_;
};

class CDXShaderFile {
 public:
  virtual void* GetBuffer() const = 0;
  virtual size_t GetBufferSize() const = 0;
  virtual const char* GetSourceName() const = 0;
  virtual const char* GetEntryPoint() const = 0;
};

class CDXShaderFactory {
 public:
  virtual bool CreateVertexShader(CDXD3DDevice* device,
                                  CDXShaderFile* sourceFile,
                                  CDXShaderFile* precompiledFile,
                                  D3D11_INPUT_ELEMENT_DESC* polygonLayout,
                                  int numElements,
                                  ComPtr<ID3D11VertexShader>& vertexShader,
                                  ComPtr<ID3D11InputLayout>& layout) {
    ComPtr<ID3D11Device> pDevice = device->GetDevice();
    ComPtr<ID3DBlob> errorMessage;
    ComPtr<ID3DBlob> shaderBuffer;

    HRESULT hr = CompileShaderFromData(
        sourceFile->GetBuffer(), sourceFile->GetBufferSize(),
        sourceFile->GetSourceName(), sourceFile->GetEntryPoint(), "vs_5_0",
        &shaderBuffer, &errorMessage);
    if (SUCCEEDED(hr)) {
      if (!shaderBuffer) {
        _ERROR("Failed to acquire vertex shader buffer");
        return false;
      }
      hr = pDevice->CreateVertexShader(shaderBuffer->GetBufferPointer(),
                                       shaderBuffer->GetBufferSize(), NULL,
                                       vertexShader.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create vertex shader");
        return false;
      }
    } else if (hr == E_NOINTERFACE && precompiledFile &&
               precompiledFile->GetBufferSize()) {
      hr = pDevice->CreateVertexShader(precompiledFile->GetBuffer(),
                                       precompiledFile->GetBufferSize(), NULL,
                                       vertexShader.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create vertex shader from precompiled data");
        return false;
      }
    } else {
      stringstream errors;
      if (errorMessage) {
        OutputShaderErrorMessage(errorMessage, errors);
      }
      _ERROR("Failed to compile vertex shader: {}", errors.str().c_str());
      return false;
    }
    return true;
  }

  virtual bool CreatePixelShader(CDXD3DDevice* device,
                                 CDXShaderFile* sourceFile,
                                 CDXShaderFile* precompiledFile,
                                 ComPtr<ID3D11PixelShader>& pixelShader) {
    ComPtr<ID3D11Device> pDevice = device->GetDevice();
    ComPtr<ID3DBlob> errorMessage;
    ComPtr<ID3DBlob> shaderBuffer;

    HRESULT hr = CompileShaderFromData(
        sourceFile->GetBuffer(), sourceFile->GetBufferSize(),
        sourceFile->GetSourceName(), sourceFile->GetEntryPoint(), "ps_5_0",
        &shaderBuffer, &errorMessage);
    if (SUCCEEDED(hr)) {
      if (!shaderBuffer) {
        _ERROR("Failed to acquire pixel shader buffer");
        return false;
      }
      hr = pDevice->CreatePixelShader(shaderBuffer->GetBufferPointer(),
                                      shaderBuffer->GetBufferSize(), NULL,
                                      pixelShader.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create pixel shader");
        return false;
      }
    } else if (hr == E_NOINTERFACE && precompiledFile &&
               precompiledFile->GetBufferSize()) {
      hr = pDevice->CreatePixelShader(precompiledFile->GetBuffer(),
                                      precompiledFile->GetBufferSize(), NULL,
                                      pixelShader.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create pixel shader from precompiled data");
        return false;
      }
    } else {
      stringstream errors;
      if (errorMessage) {
        OutputShaderErrorMessage(errorMessage, errors);
      }
      _ERROR("Failed to compile pixel shader: {}", errors.str().c_str());
      return false;
    }
    return true;
  }
  virtual bool CreateGeometryShader(
      CDXD3DDevice* device, CDXShaderFile* sourceFile,
      CDXShaderFile* precompiledFile,
      ComPtr<ID3D11GeometryShader>& geometryShader) {
    ComPtr<ID3D11Device> pDevice = device->GetDevice();
    ComPtr<ID3DBlob> errorMessage;
    ComPtr<ID3DBlob> shaderBuffer;

    HRESULT hr = CompileShaderFromData(
        sourceFile->GetBuffer(), sourceFile->GetBufferSize(),
        sourceFile->GetSourceName(), sourceFile->GetEntryPoint(), "gs_5_0",
        &shaderBuffer, &errorMessage);
    if (SUCCEEDED(hr)) {
      if (!shaderBuffer) {
        _ERROR("Failed to acquire geometry shader buffer");
        return false;
      }
      hr = pDevice->CreateGeometryShader(
          shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL,
          geometryShader.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create geometry shader");
        return false;
      }
    } else if (hr == E_NOINTERFACE && precompiledFile &&
               precompiledFile->GetBufferSize()) {
      hr = pDevice->CreateGeometryShader(
          precompiledFile->GetBuffer(), precompiledFile->GetBufferSize(), NULL,
          geometryShader.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create geometry shader from precompiled data");
        return false;
      }
    } else {
      stringstream errors;
      if (errorMessage) {
        OutputShaderErrorMessage(errorMessage, errors);
      }
      _ERROR("Failed to compile geometry shader: {}", errors.str().c_str());
      return false;
    }
    return true;
  }

 private:
  static void OutputShaderErrorMessage(ComPtr<ID3DBlob>& errorMessage,
                                       stringstream& output) {
    auto compileErrors = (char*)errorMessage->GetBufferPointer();
    const size_t bufferSize = errorMessage->GetBufferSize();
    for (size_t i = 0; i < bufferSize; i++) {
      output << compileErrors[i];
    }
    errorMessage = nullptr;
  }
};

#define DeclareFlags(type)                          \
 private:                                           \
  type flags_;                                      \
  void SetField(type uVal, type uMask, type uPos) { \
    flags_ = (flags_ & ~uMask) | (uVal << uPos);    \
  }                                                 \
  type GetField(type uMask, type uPos) const {      \
    return (flags_ & uMask) >> uPos;                \
  }                                                 \
  void SetBit(bool bVal, type uMask) {              \
    if (bVal) {                                     \
      flags_ |= uMask;                              \
    } else {                                        \
      flags_ &= ~uMask;                             \
    }                                               \
  }                                                 \
  bool GetBit(type uMask) const { return (flags_ & uMask) != 0; }

static u32 mappedAlphaFunctions[] = {
    D3D11_BLEND_ONE,          D3D11_BLEND_ZERO,
    D3D11_BLEND_SRC_COLOR,    D3D11_BLEND_INV_SRC_COLOR,
    D3D11_BLEND_DEST_COLOR,   D3D11_BLEND_INV_DEST_COLOR,
    D3D11_BLEND_SRC_ALPHA,    D3D11_BLEND_INV_SRC_ALPHA,
    D3D11_BLEND_DEST_ALPHA,   D3D11_BLEND_INV_DEST_ALPHA,
    D3D11_BLEND_SRC_ALPHA_SAT};

static u32 mappedTestFunctions[] = {
    D3D11_COMPARISON_ALWAYS,        D3D11_COMPARISON_LESS,
    D3D11_COMPARISON_EQUAL,         D3D11_COMPARISON_LESS_EQUAL,
    D3D11_COMPARISON_GREATER,       D3D11_COMPARISON_NOT_EQUAL,
    D3D11_COMPARISON_GREATER_EQUAL, D3D11_COMPARISON_NEVER,
};

class CDXMaterial {
  using Lock = std::mutex;
  using Locker = std::scoped_lock<Lock>;

 public:
  enum AlphaFunction {
    ALPHA_ONE,
    ALPHA_ZERO,
    ALPHA_SRCCOLOR,
    ALPHA_INVSRCCOLOR,
    ALPHA_DESTCOLOR,
    ALPHA_INVDESTCOLOR,
    ALPHA_SRCALPHA,
    ALPHA_INVSRCALPHA,
    ALPHA_DESTALPHA,
    ALPHA_INVDESTALPHA,
    ALPHA_SRCALPHASAT,
    ALPHA_MAX_MODES
  };

  enum {
    ALPHA_BLEND_MASK = 0x0001,
    SRC_BLEND_MASK = 0x001e,
    SRC_BLEND_POS = 1,
    DEST_BLEND_MASK = 0x01e0,
    DEST_BLEND_POS = 5,
    TEST_ENABLE_MASK = 0x0200,
    TEST_FUNC_MASK = 0x1c00,
    TEST_FUNC_POS = 10,
    ALPHA_NOSORTER_MASK = 0x2000
  };

  enum TestFunction {
    TEST_ALWAYS,
    TEST_LESS,
    TEST_EQUAL,
    TEST_LESSEQUAL,
    TEST_GREATER,
    TEST_NOTEQUAL,
    TEST_GREATEREQUAL,
    TEST_NEVER,
    TEST_MAX_MODES
  };

  CDXMaterial() {
    for (u8 i = 0; i < RE::BSTextureSet::Texture::kTotal; i++) {
      textures_[i] = nullptr;
    }
    tintColor_ = CDXColor(0.0f, 0.0f, 0.0f);
    alphaThreshold_ = 0;
    shaderFlags_ = 0;
    SetAlphaBlending(false);
    SetSrcBlendMode(ALPHA_SRCALPHA);
    SetDestBlendMode(ALPHA_INVSRCALPHA);
    SetTestMode(TEST_ALWAYS);
    blendingDirty_ = true;
    blendState_ = nullptr;
  }
  ~CDXMaterial() = default;

  void SetTexture(int index, ComPtr<ID3D11ShaderResourceView> texture) {
    Locker lock(lock_);
    if (index < 0 || index >= RE::BSTextureSet::Texture::kTotal) {
      return;
    }
    textures_[index] = texture;
  }
  ComPtr<ID3D11ShaderResourceView> GetTexture(int index) const {
    if (index < 0 || index >= RE::BSTextureSet::Texture::kTotal) {
      return nullptr;
    }
    return textures_[index];
  }
  ComPtr<ID3D11ShaderResourceView>* GetTextures() { return textures_; }
  bool HasDiffuse() const {
    return textures_[RE::BSTextureSet::Texture::kDiffuse] != nullptr;
  }
  bool HasNormal() const {
    return textures_[RE::BSTextureSet::Texture::kNormal] != nullptr;
  }
  bool HasSpecular() const {
    return textures_[RE::BSTextureSet::Texture::kSpecular] != nullptr;
  }
  bool HasTintMask() const {
    return textures_[RE::BSTextureSet::Texture::kUnused08] != nullptr;
  }
  bool HasEnvironment() const {
    return textures_[RE::BSTextureSet::Texture::kEnvironment] != nullptr;
  }
  bool HasEnvironmentMask() const {
    return textures_[RE::BSTextureSet::Texture::kEnvironmentMask] != nullptr;
  }
  bool HasDetail() const {
    return textures_[RE::BSTextureSet::Texture::kDetailMap] != nullptr;
  }
  bool HasGlow() const {
    return textures_[RE::BSTextureSet::Texture::kGlowMap] != nullptr;
  }

  ComPtr<ID3D11BlendState> GetBlendState(CDXD3DDevice* device) {
    if (blendingDirty_) {
      D3D11_BLEND_DESC desc;
      ZeroMemory(&desc, sizeof(desc));
      desc.RenderTarget[0].BlendEnable = GetAlphaBlending();
      desc.RenderTarget[0].SrcBlend = GetD3DBlendMode(GetSrcBlendMode());
      desc.RenderTarget[0].DestBlend = GetD3DBlendMode(GetDestBlendMode());
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

      HRESULT hr = device->GetDevice()->CreateBlendState(
          &desc, blendState_.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create blend state");
        return nullptr;
      }
      blendingDirty_ = false;
    }
    return blendState_;
  }
  void SetTintColor(CDXColor color) {
    Locker lock(lock_);
    tintColor_ = color;
  }
  CDXColor& GetTintColor() {
    Locker lock(lock_);
    return tintColor_;
  }

  u64 GetShaderFlags() {
    Locker lock(lock_);
    return shaderFlags_;
  }
  void SetShaderFlags(u64 flags) {
    Locker lock(lock_);
    shaderFlags_ = flags;
  }

  void SetFlags(u16 flags) {
    Locker lock(lock_);
    flags_ = flags;
  }

  float GetAlphaThreshold() const { return alphaThreshold_; }

  bool GetAlphaBlending() const { return GetBit(ALPHA_BLEND_MASK); }
  void SetAlphaBlending(bool alphaBlend) {
    SetBit(alphaBlend, ALPHA_BLEND_MASK);
    blendingDirty_ = true;
  }
  AlphaFunction GetSrcBlendMode() const {
    return (AlphaFunction)GetField(SRC_BLEND_MASK, SRC_BLEND_POS);
  }
  void SetSrcBlendMode(AlphaFunction srcBlend) {
    SetField((u16)srcBlend, SRC_BLEND_MASK, SRC_BLEND_POS);
    blendingDirty_ = true;
  }
  AlphaFunction GetDestBlendMode() const {
    return (AlphaFunction)GetField(DEST_BLEND_MASK, DEST_BLEND_POS);
  }
  void SetDestBlendMode(AlphaFunction destBlend) {
    SetField((u16)destBlend, DEST_BLEND_MASK, DEST_BLEND_POS);
    blendingDirty_ = true;
  }
  bool GetAlphaTesting() const { return GetBit(TEST_ENABLE_MASK); }
  void SetAlphaTesting(bool alphaTest) { SetBit(alphaTest, TEST_ENABLE_MASK); }
  TestFunction GetTestMode() const {
    return (TestFunction)GetField(TEST_FUNC_MASK, TEST_FUNC_POS);
  }
  void SetTestMode(TestFunction testFunc) {
    SetField((u16)testFunc, TEST_FUNC_MASK, TEST_FUNC_POS);
  }

  static D3D11_BLEND GetD3DBlendMode(AlphaFunction alphaFunc) {
    static const unordered_map<AlphaFunction, D3D11_BLEND> TEST_MODE = {
        {ALPHA_ONE, D3D11_BLEND_ONE},
        {ALPHA_ZERO, D3D11_BLEND_ZERO},
        {ALPHA_SRCCOLOR, D3D11_BLEND_SRC_COLOR},
        {ALPHA_INVSRCCOLOR, D3D11_BLEND_INV_SRC_COLOR},
        {ALPHA_DESTCOLOR, D3D11_BLEND_DEST_COLOR},
        {ALPHA_INVDESTCOLOR, D3D11_BLEND_INV_DEST_COLOR},
        {ALPHA_SRCALPHA, D3D11_BLEND_SRC_ALPHA},
        {ALPHA_INVSRCALPHA, D3D11_BLEND_INV_SRC_ALPHA},
        {ALPHA_DESTALPHA, D3D11_BLEND_DEST_ALPHA},
        {ALPHA_INVDESTALPHA, D3D11_BLEND_INV_DEST_ALPHA},
        {ALPHA_SRCALPHASAT, D3D11_BLEND_SRC_ALPHA_SAT}};
    if (auto it = TEST_MODE.find(alphaFunc); it != TEST_MODE.end()) {
      return it->second;
    }
    return D3D11_BLEND_ZERO;
  }

 protected:
  ComPtr<ID3D11ShaderResourceView> textures_[RE::BSTextureSet::Texture::kTotal];
  ComPtr<ID3D11BlendState> blendState_;
  bool blendingDirty_;

  CDXColor tintColor_;
  DeclareFlags(u16);
  u64 shaderFlags_;
  u8 alphaThreshold_;
  Lock lock_;
};

class CDXShader {
 public:
  CDXShader() = default;
  bool Initialize(const CDXInitParams& initParams) {
    D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC bufferDesc;

    auto pDevice = initParams.device->GetDevice();

    // Create the vertex input layout description.
    // This setup needs to match the VertexType stucture in the ModelClass and
    // in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    polygonLayout[3].SemanticName = "COLOR";
    polygonLayout[3].SemanticIndex = 0;
    polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[3].InputSlot = 0;
    polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[3].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    u32 numElements = std::size(polygonLayout);

    // LightVertexShader
    if (!initParams.factory->CreateVertexShader(
            initParams.device, initParams.vShader[0], initParams.vShader[1],
            polygonLayout, numElements, vertexShader_, layout_)) {
      return false;
    }

    // LightPixelShader
    if (!initParams.factory->CreatePixelShader(
            initParams.device, initParams.pShader[0], initParams.pShader[1],
            pixelShader_)) {
      return false;
    }

    // Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    HRESULT hr = pDevice->CreateSamplerState(
        &samplerDesc, samplerState_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      _ERROR("%s - Failed to create sampler state", __FUNCTION__);
      return false;
    }

    // Setup the description of the matrix dynamic constant buffer that is in
    // the vertex shader.
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.ByteWidth = sizeof(VertexBuffer);

    // Create the matrix constant buffer pointer so we can access the vertex
    // shader constant buffer from within this class.
    hr = pDevice->CreateBuffer(&bufferDesc, NULL,
                               matrixBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      _ERROR("%s - Failed to create matrix buffer", __FUNCTION__);
      return false;
    }

    bufferDesc.ByteWidth = sizeof(TransformBuffer);

    // Create the matrix constant buffer pointer so we can access the vertex
    // shader constant buffer from within this class.
    hr = pDevice->CreateBuffer(&bufferDesc, NULL,
                               transformBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      _ERROR("%s - Failed to create transform buffer", __FUNCTION__);
      return false;
    }

    // Setup the description of the light dynamic constant buffer that is in the
    // pixel shader. Note that ByteWidth always needs to be a multiple of 16 if
    // using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
    bufferDesc.ByteWidth = sizeof(MaterialBuffer);

    // Create the constant buffer pointer so we can access the vertex shader
    // constant buffer from within this class.
    hr = pDevice->CreateBuffer(&bufferDesc, NULL,
                               materialBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      _ERROR("%s - Failed to create material buffer", __FUNCTION__);
      return false;
    }

    // Setup the raster description which will determine how and what polygons
    // will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    hr = pDevice->CreateRasterizerState(&rasterDesc,
                                        solidState_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      _ERROR("%s - Failed to create solid rasterizer state", __FUNCTION__);
      return false;
    }

    return true;
  }
  struct VertexBuffer {
    CDXMatrix world, view, projection, viewport;
  };
  struct TransformBuffer {
    CDXMatrix transform;
  };
  struct MaterialBuffer {
    CDXColor tintColor;
    int hasNormal, hasSpecular, hasDetail, hasTintMask, hasEnvironment,
        hasEnvironmentMask, hasGlowMap;
    float alphaThreshold, padding[3];
  };

  void RenderShader(CDXD3DDevice* device,
                    const shared_ptr<CDXMaterial>& material) {
    auto deviceContext = device->GetDeviceContext();
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(layout_.Get());
    MaterialBuffer mat;

    auto* state = solidState_.Get();
    auto* vertexShader = vertexShader_.Get();
    auto* pixelShader = pixelShader_.Get();
    ID3D11GeometryShader* geometryShader = nullptr;
    auto* blendState = material->GetBlendState(device).Get();
    constexpr float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    mat.hasNormal = material->HasNormal();
    mat.hasSpecular = material->HasSpecular();
    mat.hasDetail = material->HasDetail();
    mat.hasTintMask = material->HasTintMask();
    mat.hasEnvironment = material->HasEnvironment();
    mat.hasEnvironmentMask = material->HasEnvironmentMask();
    mat.hasGlowMap = material->HasGlow();
    mat.tintColor = material->GetTintColor();
    mat.alphaThreshold = material->GetAlphaBlending()
                             ? 0.0f
                             : material->GetAlphaThreshold() / 255.0f;

    PSSetMaterialBuffers(device, mat);
    deviceContext->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
    deviceContext->RSSetState(state);
    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->GSSetShader(geometryShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);

    auto textures = material->GetTextures();
    ID3D11ShaderResourceView* resources[] = {
        textures[RE::BSTextureSet::Texture::kDiffuse].Get(),
        textures[RE::BSTextureSet::Texture::kNormal].Get(),
        textures[RE::BSTextureSet::Texture::kEnvironmentMask].Get(),
        textures[RE::BSTextureSet::Texture::kDetailMap].Get(),
        nullptr,
        textures[RE::BSTextureSet::Texture::kEnvironment].Get(),
        nullptr,
        textures[RE::BSTextureSet::Texture::kSpecular].Get(),
        textures[RE::BSTextureSet::Texture::kUnused08].Get(),
    };
    deviceContext->PSSetShaderResources(0, RE::BSTextureSet::Texture::kTotal,
                                        resources);
    ID3D11SamplerState* samplers[] = {samplerState_.Get()};
    deviceContext->PSSetSamplers(0, 1, samplers);
  }

  bool VSSetShaderBuffer(CDXD3DDevice* device, VertexBuffer& params) const {
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    auto deviceContext = device->GetDeviceContext();
    // Lock the constant buffer so it can be written to.
    HRESULT hr = deviceContext->Map(
        matrixBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
      _ERROR("Failed to map matrix buffer");
      return false;
    }

    auto* dataPtr = (VertexBuffer*)mappedResource.pData;
    params.world = DirectX::XMMatrixTranspose(params.world);
    params.view = DirectX::XMMatrixTranspose(params.view);
    params.projection = DirectX::XMMatrixTranspose(params.projection);

    *dataPtr = params;
    deviceContext->Unmap(matrixBuffer_.Get(), 0);
    ID3D11Buffer* buffers[] = {matrixBuffer_.Get()};
    deviceContext->VSSetConstantBuffers(0, 1, buffers);
    deviceContext->GSSetConstantBuffers(0, 1, buffers);
    return true;
  }

  bool VSSetTransformBuffer(CDXD3DDevice* device,
                            TransformBuffer& params) const {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto deviceContext = device->GetDeviceContext();

    HRESULT hr = deviceContext->Map(
        transformBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
      _ERROR("Failed to map transform buffer");
      return false;
    }

    auto* dataPtr = (TransformBuffer*)mappedResource.pData;
    *dataPtr = params;
    deviceContext->Unmap(transformBuffer_.Get(), 0);
    ID3D11Buffer* buffers[] = {transformBuffer_.Get()};
    deviceContext->VSSetConstantBuffers(1, 1, buffers);
    return true;
  }

  bool PSSetMaterialBuffers(CDXD3DDevice* device,
                            MaterialBuffer& params) const {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto deviceContext = device->GetDeviceContext();
    HRESULT hr = deviceContext->Map(
        materialBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
      _ERROR("Failed to map material buffer");
      return false;
    }
    auto* dataPtr = (MaterialBuffer*)mappedResource.pData;
    *dataPtr = params;
    deviceContext->Unmap(materialBuffer_.Get(), 0);
    ID3D11Buffer* buffers[] = {materialBuffer_.Get()};
    deviceContext->PSSetConstantBuffers(0, 1, buffers);
    return true;
  }

 protected:
  ComPtr<ID3D11VertexShader> vertexShader_;
  ComPtr<ID3D11PixelShader> pixelShader_;
  ComPtr<ID3D11DepthStencilState> depthStencilState_;
  UINT baseRef_;
  ComPtr<ID3D11DepthStencilState> baseStencilState_;
  ComPtr<ID3D11InputLayout> layout_;
  ComPtr<ID3D11SamplerState> samplerState_;
  ComPtr<ID3D11RasterizerState> solidState_;
  ComPtr<ID3D11Buffer> matrixBuffer_;
  ComPtr<ID3D11Buffer> transformBuffer_;
  ComPtr<ID3D11Buffer> materialBuffer_;
};

class CDXTexturePixelShaderFile : public CDXShaderFile {
 public:
  CDXTexturePixelShaderFile(const string& shaderName);

  virtual void* GetBuffer() const override { return (void*)buffer_.data(); }
  virtual size_t GetBufferSize() const override { return buffer_.size(); }
  virtual const char* GetSourceName() const override { return name_.c_str(); }
  virtual const char* GetEntryPoint() const override {
    return entryPoint_.c_str();
  }

 private:
  string name_;
  string entryPoint_;
  vector<char> buffer_;
};

class CDXBSShaderResource : public CDXShaderFile {
 public:
  CDXBSShaderResource(const char* path, const char* entryPoint = "") {
    const char* shaderPath = "SKSE/Plugins/NiOverride/texture.fx";
    vector<char> vsb;
    if (RE::BSResourceNiBinaryStream vs(path); !vs.good()) {
      _ERROR("Failed to open shader file: {}", path);
    } else {
      const size_t size = vs.tell();
      buffer_.resize(size);
      vs.read(buffer_.data(), size);
      name_ = path;
      entryPoint_ = entryPoint;
    }
  }

  virtual void* GetBuffer() const {
    return buffer_.empty() ? nullptr : (void*)buffer_.at(0);
  }
  virtual size_t GetBufferSize() const { return buffer_.size(); }
  virtual const char* GetSourceName() const { return name_.c_str(); }
  virtual const char* GetEntryPoint() const { return entryPoint_.c_str(); }

 private:
  string name_;
  string entryPoint_;
  vector<char> buffer_;
};

class CDXNifPixelShaderCache : public CDXPixelShaderCache {
 public:
  explicit CDXNifPixelShaderCache(CDXShaderFactory* factory)
      : CDXPixelShaderCache(factory) {}
  ComPtr<ID3D11PixelShader> GetShader(CDXD3DDevice* device,
                                      const string& name) override {
    string xform = name;
    ranges::transform(xform, xform.begin(), ::tolower);
    ComPtr<ID3D11PixelShader> shader = __super::GetShader(device, name);
    if (shader) {
      return shader;
    }
    char shaderPath[MAX_PATH];
    sprintf_s(shaderPath, "SKSE/Plugins/NiOverride/Shaders/Effects/%s.fx",
              name.c_str());
    CDXBSShaderResource shaderFile(shaderPath, name.c_str());
    sprintf_s(shaderPath, MAX_PATH,
              "SKSE/Plugins/NiOverride/Shaders/Compiled/Effects/%s.cso",
              name.c_str());
    CDXBSShaderResource compiledFile(shaderPath, name.c_str());
    if (factory_->CreatePixelShader(device, &shaderFile, &compiledFile,
                                    shader)) {
      insert_or_assign(name, shader);
      return shader;
    }
    return nullptr;
  }
};

class CDXTextureRenderer {
 public:
  struct VertexType {
    CDXVec3 position;
    CDXVec2 texture;
  };

  enum class TextureType : u8 {
    Normal = 0,
    Mask,
    Color,
    Unknown,
  };

  struct LayerData {
    union Mode {
      struct BlendState {
        u32 dummy;
        u32 type;
      } blendData;
      DirectX::XMINT2 data;
    } blending;
    CDXVec4 maskColor;
  };

  struct ConstBufferType {
    CDXMatrix world, view, projection;
    LayerData layerData;
  };

  CDXTextureRenderer() {
    shaderCache_ = nullptr;
    source_ = nullptr;
    ZeroMemory(&srcDesc_, sizeof(srcDesc_));
    ZeroMemory(&dstDesc_, sizeof(dstDesc_));
    dstDesc_.Width = 8;
    dstDesc_.Height = 8;
    vertexBuffer_ = nullptr;
    indexBuffer_ = nullptr;
    constBuffer_ = nullptr;
    vertexShader_ = nullptr;
    layout_ = nullptr;
    samplerState_ = nullptr;
    alphaEnabledBlendState_ = nullptr;
    renderTargetTexture_ = nullptr;
    renderTargetView_ = nullptr;
    shaderResourceView_ = nullptr;
  }
  virtual ~CDXTextureRenderer() = default;

  virtual bool Initialize(CDXD3DDevice* device, CDXShaderFactory* factory,
                          CDXShaderFile* sourceFile,
                          CDXShaderFile* precompiledFile,
                          CDXPixelShaderCache* cache) {
    shaderCache_ = cache;
    if (!InitializeVertices(device)) {
      return false;
    }
    if (!InitializeVertexShader(device, factory, sourceFile, precompiledFile)) {
      return false;
    }
    auto pDevice = device->GetDevice();
    D3D11_BLEND_DESC blendStateDesc;
    ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
    blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = pDevice->CreateBlendState(
        &blendStateDesc, alphaEnabledBlendState_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      return false;
    }
    return true;
  }

  virtual void Render(CDXD3DDevice* device, bool clear = true) {
    u32 stride = sizeof(VertexType);
    u32 offset = 0;
    auto pDevice = device->GetDevice();
    auto deviceContext = device->GetDeviceContext();

    if (source_) {
      if (!UpdateConstBuffer(device)) {
        return;
      }
      D3D11_VIEWPORT viewport;
      viewport.Width = (FLOAT)GetWidth();
      viewport.Height = (FLOAT)GetHeight();
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      deviceContext->RSSetViewports(1, &viewport);

      ID3D11RenderTargetView* renderTargets[] = {renderTargetView_.Get()};
      deviceContext->OMSetRenderTargets(1, renderTargets,
                                        nullptr);  // No depth buffer
      constexpr float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
      deviceContext->OMSetBlendState(alphaEnabledBlendState_.Get(), blendFactor,
                                     0xFFFFFFFF);
      constexpr float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
      if (clear) {
        deviceContext->ClearRenderTargetView(renderTargetView_.Get(), color);
      }

      ID3D11Buffer* vertexBuffer[] = {vertexBuffer_.Get()};
      deviceContext->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);
      deviceContext->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT,
                                      0);
      deviceContext->IASetPrimitiveTopology(
          D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      deviceContext->IASetInputLayout(layout_.Get());

      ID3D11Buffer* constBuffer[] = {constBuffer_.Get()};
      deviceContext->VSSetConstantBuffers(0, 1, constBuffer);
      deviceContext->VSSetShader(vertexShader_.Get(), NULL, 0);

      ID3D11SamplerState* samplers[] = {samplerState_.Get()};
      deviceContext->PSSetSamplers(0, 1, samplers);

      vector<ComPtr<ID3D11ShaderResourceView>> srvs;
      if (SplitSubresources(device, srcDesc_, source_, srvs)) {
        for (size_t i = 0; i < srvs.size(); ++i) {
          RenderShaders(device, srvs[i]);
          D3D11_BOX srcRegion;
          srcRegion.left = 0;
          srcRegion.top = 0;
          srcRegion.front = 0;
          srcRegion.right = dstDesc_.Width;
          srcRegion.bottom = dstDesc_.Height;
          srcRegion.back = 1;

          deviceContext->CopySubresourceRegion(
              multiTexture_.Get(), D3D11CalcSubresource(0, i, 1), 0, 0, 0,
              renderTargetTexture_.Get(), D3D11CalcSubresource(0, 0, 1),
              &srcRegion);
        }
      } else {
        RenderShaders(device, source_);
      }
    }
  }

  virtual void Release() {
    source_ = nullptr;
    indexBuffer_ = nullptr;
    vertexBuffer_ = nullptr;
    constBuffer_ = nullptr;
    samplerState_ = nullptr;
    layout_ = nullptr;
    vertexShader_ = nullptr;
    alphaEnabledBlendState_ = nullptr;
    renderTargetView_ = nullptr;
    shaderResourceView_ = nullptr;
    renderTargetTexture_ = nullptr;
    intermediateSRV_ = nullptr;
    intermediateTexture_ = nullptr;
    multiSRV_ = nullptr;
    multiTexture_ = nullptr;
  }

  virtual bool SetTexture(CDXD3DDevice* device,
                          ComPtr<ID3D11ShaderResourceView> texture,
                          DXGI_FORMAT target) {
    source_ = texture;
    if (source_) {
      auto pDevice = device->GetDevice();
      ComPtr<ID3D11Resource> resource;
      source_->GetResource(&resource);
      ComPtr<ID3D11Texture2D> pTexture;
      HRESULT hr = resource.As(&pTexture);
      if (FAILED(hr)) {
        _ERROR("Failed to get texture resource");
        return false;
      }
      pTexture->GetDesc(&srcDesc_);
      if (srcDesc_.Width != dstDesc_.Width ||
          srcDesc_.Height != dstDesc_.Height ||
          srcDesc_.ArraySize != dstDesc_.ArraySize) {
        renderTargetTexture_ = nullptr;
        renderTargetView_ = nullptr;
        shaderResourceView_ = nullptr;
        intermediateTexture_ = nullptr;
        intermediateSRV_ = nullptr;
        multiTexture_ = nullptr;
        multiSRV_ = nullptr;
      } else {
        return true;
      }

      ZeroMemory(&dstDesc_, sizeof(dstDesc_));
      dstDesc_.Width = srcDesc_.Width;
      dstDesc_.Height = srcDesc_.Height;
      dstDesc_.MipLevels = 1;
      dstDesc_.ArraySize = 1;
      dstDesc_.Format = target;
      dstDesc_.SampleDesc.Count = 1;
      dstDesc_.SampleDesc.Quality = 0;
      dstDesc_.Usage = D3D11_USAGE_DEFAULT;
      dstDesc_.BindFlags =
          D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
      dstDesc_.CPUAccessFlags = 0;
      dstDesc_.MiscFlags = 0;

      if (!UpdateVertexBuffer(device)) {
        _ERROR("Failed to update vertex buffer");
        return false;
      }
      hr = pDevice->CreateTexture2D(
          &dstDesc_, NULL, renderTargetTexture_.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create render target texture");
        return false;
      }
      hr = pDevice->CreateTexture2D(
          &dstDesc_, NULL, intermediateTexture_.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create intermediate texture");
        return false;
      }

      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
      rtvDesc.Format = dstDesc_.Format;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      rtvDesc.Texture2D.MipSlice = 0;
      hr = pDevice->CreateRenderTargetView(
          renderTargetTexture_.Get(), &rtvDesc,
          renderTargetView_.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create render target view");
        return false;
      }
      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
      srvDesc.Format = dstDesc_.Format;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      srvDesc.Texture2D.MostDetailedMip = 0;
      srvDesc.Texture2D.MipLevels = 1;
      hr = pDevice->CreateShaderResourceView(
          renderTargetTexture_.Get(), &srvDesc,
          shaderResourceView_.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create shader resource view");
        return false;
      }
      hr = pDevice->CreateShaderResourceView(
          intermediateTexture_.Get(), &srvDesc,
          intermediateSRV_.ReleaseAndGetAddressOf());
      if (FAILED(hr)) {
        _ERROR("Failed to create intermediate shader resource view");
        return false;
      }
      if (srcDesc_.ArraySize > 1) {
        if (!CreateSubresourceDestination(device, srcDesc_, multiTexture_,
                                          multiSRV_)) {
          _ERROR("Failed to create multi texture");
          return false;
        }
      }
      return true;
    }
    return false;
  }

  bool UpdateVertexBuffer(CDXD3DDevice* device) {
    if (!vertexBuffer_) {
      return false;
    }
    auto deviceContext = device->GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    unique_ptr<VertexType[]> vertices;

    // Calculate the screen coordinates of the left side of the bitmap.
    float left = (float)((GetWidth() / 2) * -1);

    // Calculate the screen coordinates of the right side of the bitmap.
    float right = left + (float)GetWidth();

    // Calculate the screen coordinates of the top of the bitmap.
    float top = (float)(GetHeight() / 2);

    // Calculate the screen coordinates of the bottom of the bitmap.
    float bottom = top - (float)GetHeight();

    // Create the vertex array.
    vertices = make_unique<VertexType[]>(vertexCount_);
    if (!vertices) {
      return false;
    }

    // Load the vertex array with data.
    // First triangle.
    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[1].position =
        DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[1].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    vertices[2].position =
        DirectX::XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
    vertices[2].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

    // Second triangle.
    vertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[3].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);  // Top right.
    vertices[4].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

    vertices[5].position =
        DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[5].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    // Lock the vertex buffer so it can be written to.
    HRESULT hr = deviceContext->Map(
        vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
      return false;
    }

    // Get a pointer to the data in the vertex buffer.
    auto* verticesPtr = (VertexType*)mappedResource.pData;

    // Copy the data into the vertex buffer.
    memcpy(verticesPtr, vertices.get(), sizeof(VertexType) * vertexCount_);

    // Unlock the vertex buffer.
    deviceContext->Unmap(vertexBuffer_.Get(), 0);

    return true;
  }

  bool UpdateConstBuffer(CDXD3DDevice* device) {
    if (!constBuffer_) {
      return false;
    }
    auto deviceContext = device->GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(
        constBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
      return false;
    }

    // Get a pointer to the data in the vertex buffer.
    auto* dataPtr = (ConstBufferType*)mappedResource.pData;
    dataPtr->world = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
    dataPtr->view = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
    auto projectionMatrix = DirectX::XMMatrixOrthographicLH(
        static_cast<float>(GetWidth()), static_cast<float>(GetHeight()),
        1000.0f, 0.1f);
    projectionMatrix.r[3].m128_f32[3] =
        1.0001f;  // Fudge the W argument to make it show
    dataPtr->projection = XMMatrixTranspose(projectionMatrix);

    // Unlock the vertex buffer.
    deviceContext->Unmap(constBuffer_.Get(), 0);

    return true;
  }

  bool UpdateStructBuffer(CDXD3DDevice* device, const LayerData& data) {
    if (!structBuffer_) {
      return false;
    }
    auto deviceContext = device->GetDeviceContext();
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(
        structBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
      return false;
    }

    // Get a pointer to the data in the vertex buffer.
    auto* dataPtr = (LayerData*)mappedResource.pData;
    memcpy(dataPtr, &data, sizeof(LayerData));

    // Unlock the vertex buffer.
    deviceContext->Unmap(structBuffer_.Get(), 0);

    return true;
  }

  int GetWidth() const { return dstDesc_.Width; }
  int GetHeight() const { return dstDesc_.Height; }

  ComPtr<ID3D11Texture2D> GetTexture() {
    if (srcDesc_.ArraySize > 1) {
      return multiTexture_;
    }
    return renderTargetTexture_;
  }

  ComPtr<ID3D11ShaderResourceView> GetResourceView() {
    if (srcDesc_.ArraySize > 1) {
      return multiSRV_;
    }
    return shaderResourceView_;
  }

  void AddLayer(const ComPtr<ID3D11ShaderResourceView>& texture,
                const TextureType& type, const string& technique,
                const CDXVec4& maskColor) {
    resources_.push_back(
        {{0, static_cast<u32>(type), maskColor}, texture, technique});
  }

 protected:
  struct ResourceData {
    LayerData metadata;
    ComPtr<ID3D11ShaderResourceView> resource;
    string shader;
  };

  CDXPixelShaderCache* shaderCache_;
  ComPtr<ID3D11ShaderResourceView> source_;
  vector<ResourceData> resources_;
  ComPtr<ID3D11Buffer> vertexBuffer_;
  ComPtr<ID3D11Buffer> indexBuffer_;
  ComPtr<ID3D11Buffer> constBuffer_;
  ComPtr<ID3D11Buffer> structBuffer_;
  ComPtr<ID3D11ShaderResourceView> structBufferView_;
  ComPtr<ID3D11VertexShader> vertexShader_;
  ComPtr<ID3D11InputLayout> layout_;
  ComPtr<ID3D11SamplerState> samplerState_;
  ComPtr<ID3D11BlendState> alphaEnabledBlendState_;
  ComPtr<ID3D11Texture2D> renderTargetTexture_;
  ComPtr<ID3D11RenderTargetView> renderTargetView_;
  ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
  ComPtr<ID3D11Texture2D> intermediateTexture_;
  ComPtr<ID3D11ShaderResourceView> intermediateSRV_;
  ComPtr<ID3D11Texture2D> multiTexture_;
  ComPtr<ID3D11ShaderResourceView> multiSRV_;

  int vertexCount_{0}, indexCount_{0};
  D3D11_TEXTURE2D_DESC srcDesc_, dstDesc_;

  bool InitializeVertices(CDXD3DDevice* device) {
    unique_ptr<VertexType[]> vertices;
    unique_ptr<unsigned long[]> indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

    auto pDevice = device->GetDevice();

    vertexCount_ = 6;

    indexCount_ = vertexCount_;

    // Create the vertex array.
    vertices = std::make_unique<VertexType[]>(vertexCount_);
    if (!vertices) {
      return false;
    }

    indices = std::make_unique<unsigned long[]>(indexCount_);
    if (!indices) {
      return false;
    }

    memset(vertices.get(), 0, (sizeof(VertexType) * vertexCount_));

    for (int i = 0; i < indexCount_; i++) {
      indices[i] = i;
    }

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount_;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices.get();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData,
                                       vertexBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      return false;
    }

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount_;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices.get();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    hr = pDevice->CreateBuffer(&indexBufferDesc, &indexData,
                               indexBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
      return false;
    }

    return true;
  }
  bool InitializeVertexShader(CDXD3DDevice* device, CDXShaderFactory* factory,
                              CDXShaderFile* sourceFile,
                              CDXShaderFile* precompiledFile) {
    HRESULT result;
    ComPtr<ID3D11Device> pDevice = device->GetDevice();
    std::stringstream errors;

    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    unsigned int numElements = size(polygonLayout);

    if (!factory->CreateVertexShader(device, sourceFile, precompiledFile,
                                     polygonLayout, numElements, vertexShader_,
                                     layout_)) {
      return false;
    }

    // Setup the description of the dynamic constant buffer that is in the
    // vertex shader.
    D3D11_BUFFER_DESC constBufferDesc;
    constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufferDesc.ByteWidth = sizeof(ConstBufferType);
    constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufferDesc.MiscFlags = 0;
    constBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader
    // constant buffer from within this class.
    result = pDevice->CreateBuffer(&constBufferDesc, nullptr,
                                   constBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(result)) {
      return false;
    }

    D3D11_BUFFER_DESC sBufferDesc;
    sBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    sBufferDesc.ByteWidth = sizeof(LayerData);
    sBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    sBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    sBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    sBufferDesc.StructureByteStride = sizeof(LayerData);
    result = pDevice->CreateBuffer(&sBufferDesc, nullptr,
                                   structBuffer_.ReleaseAndGetAddressOf());
    if (FAILED(result)) {
      return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = 1;
    result = pDevice->CreateShaderResourceView(
        structBuffer_.Get(), &srvDesc,
        structBufferView_.ReleaseAndGetAddressOf());
    if (FAILED(result)) {
      return false;
    }

    // Create a texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    result = pDevice->CreateSamplerState(&samplerDesc, &samplerState_);
    if (FAILED(result)) {
      return false;
    }

    return true;
  }
  void RenderShaders(CDXD3DDevice* device,
                     ComPtr<ID3D11ShaderResourceView> sourceView) const {
    auto pDevice = device->GetDevice();
    auto deviceContext = device->GetDeviceContext();

    size_t i = 0;
    ID3D11ShaderResourceView* srv = sourceView.Get();
    do {
      auto& res = resources_[i];
      auto shader = shaderCache_->GetShader(device, res.shader);
      if (!shader) {
        shader = shaderCache_->GetShader(device, "normal");
      }
      ID3D11ShaderResourceView* resources[] = {srv, res.resource.Get(),
                                               structBufferView_.Get()};
      deviceContext->PSSetShader(shader.Get(), nullptr, 0);
      deviceContext->PSSetShaderResources(0, 3, resources);
      deviceContext->DrawIndexed(indexCount_, 0, 0);
      deviceContext->CopyResource(intermediateTexture_.Get(),
                                  renderTargetTexture_.Get());
      srv = intermediateSRV_.Get();
      i++;
    } while (i < resources_.size());
  }

  bool SplitSubresources(CDXD3DDevice* device, D3D11_TEXTURE2D_DESC desc,
                         ComPtr<ID3D11ShaderResourceView> source,
                         vector<ComPtr<ID3D11ShaderResourceView>>& resources) {
    if (desc.ArraySize > 1) {
      auto pDevice = device->GetDevice();
      auto deviceContext = device->GetDeviceContext();

      ComPtr<ID3D11Resource> pResource;
      source->GetResource(&pResource);

      D3D11_TEXTURE2D_DESC cubeMapDesc = desc;
      cubeMapDesc.MipLevels = 1;
      cubeMapDesc.ArraySize = 1;
      cubeMapDesc.SampleDesc.Count = 1;
      cubeMapDesc.SampleDesc.Quality = 0;
      cubeMapDesc.CPUAccessFlags = 0;
      cubeMapDesc.MiscFlags = 0;

      D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
      shaderResourceViewDesc.Format = cubeMapDesc.Format;
      shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
      shaderResourceViewDesc.Texture2D.MipLevels = 1;

      for (UINT i = 0; i < desc.ArraySize; ++i) {
        // Create the render target texture.
        ComPtr<ID3D11Texture2D> cubeSideTexture;
        HRESULT result = pDevice->CreateTexture2D(
            &cubeMapDesc, NULL, cubeSideTexture.ReleaseAndGetAddressOf());
        if (FAILED(result)) {
          _ERROR("Failed to create cube side texture");
          return false;
        }

        ComPtr<ID3D11ShaderResourceView> resourceView;
        result = pDevice->CreateShaderResourceView(
            cubeSideTexture.Get(), &shaderResourceViewDesc,
            resourceView.ReleaseAndGetAddressOf());
        if (FAILED(result)) {
          _ERROR("Failed to create temporary cube map resource view");
          return false;
        }

        D3D11_BOX sourceRegion;
        sourceRegion.left = 0;
        sourceRegion.right = desc.Width;
        sourceRegion.top = 0;
        sourceRegion.bottom = desc.Height;
        sourceRegion.front = 0;
        sourceRegion.back = 1;

        deviceContext->CopySubresourceRegion(
            cubeSideTexture.Get(), D3D11CalcSubresource(0, 0, 1), 0, 0, 0,
            pResource.Get(), D3D11CalcSubresource(0, i, desc.MipLevels),
            &sourceRegion);

        resources.push_back(resourceView);
      }

      return true;
    }

    return false;
  }

  bool CreateSubresourceDestination(
      CDXD3DDevice* device, D3D11_TEXTURE2D_DESC desc,
      ComPtr<ID3D11Texture2D>& outTexture,
      ComPtr<ID3D11ShaderResourceView>& outResource) {
    auto pDevice = device->GetDevice();
    auto deviceContext = device->GetDeviceContext();

    D3D11_TEXTURE2D_DESC cubeMapDesc = desc;
    cubeMapDesc.Format = dstDesc_.Format;
    cubeMapDesc.MipLevels = 1;
    cubeMapDesc.ArraySize = desc.ArraySize;
    cubeMapDesc.SampleDesc.Count = 1;
    cubeMapDesc.SampleDesc.Quality = 0;
    cubeMapDesc.CPUAccessFlags = 0;
    cubeMapDesc.MiscFlags = (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
                                ? D3D11_RESOURCE_MISC_TEXTURECUBE
                                : 0;

    // Create the render target texture.
    HRESULT result = pDevice->CreateTexture2D(
        &cubeMapDesc, NULL, outTexture.ReleaseAndGetAddressOf());
    if (FAILED(result)) {
      _ERROR("%s - Failed to create merged texture", __FUNCTION__);
      return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = cubeMapDesc.Format;
    shaderResourceViewDesc.ViewDimension =
        (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
            ? D3D11_SRV_DIMENSION_TEXTURECUBE
            : D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = pDevice->CreateShaderResourceView(
        outTexture.Get(), &shaderResourceViewDesc,
        outResource.ReleaseAndGetAddressOf());
    if (FAILED(result)) {
      _ERROR("%s - Failed to create cubemap resource view", __FUNCTION__);
      return false;
    }

    return true;
  }
};
