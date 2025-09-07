#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <memory>
#include <sstream>

using namespace DirectX;

namespace Graphics {
class PixelShaderCache;
class D3DDevice;
class ShaderFile;
class ShaderFactory;

class TextureRenderer {
 public:
  struct VertexType {
    XMFLOAT3 position;
    XMFLOAT2 texture;
  };

  enum class TextureType { kNormal = 0, kMask, kColor, kUnknown };

  struct LayerData {
    union Mode {
      struct BlendData {
        UINT32 dummy;
        UINT32 type;
      } blendData;
      XMINT2 data;
    } blending;
    XMFLOAT4 maskColor;
  };

  struct ConstantBufferType {
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
    LayerData layerData;
  };

  TextureRenderer();
  virtual ~TextureRenderer() {}

  virtual bool Initialize(D3DDevice* device, ShaderFactory* shaderFactory,
                          ShaderFile* sourceFile, ShaderFile* precompiledFile,
                          PixelShaderCache* cache);
  virtual void Render(D3DDevice* device, bool clear = true);
  virtual void Release();
  virtual bool SetTexture(
      D3DDevice* device,
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
      DXGI_FORMAT target);

  bool UpdateVertexBuffer(D3DDevice* device);
  bool UpdateConstantBuffer(D3DDevice* device);
  bool UpdateStructureBuffer(D3DDevice* device, const LayerData& layerData);

 protected:
  bool InitializeVertices(D3DDevice* device);
  bool InitializeVertexShader(D3DDevice* device, ShaderFactory* shaderFactory,
                              ShaderFile* sourceFile,
                              ShaderFile* precompiledFile);

  PixelShaderCache* shaderCache_;

  void RenderShaders(
      D3DDevice* device,
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sourceView);

  bool SplitSubresources(
      D3DDevice* device, D3D11_TEXTURE2D_DESC desc,
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> source,
      std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& resources);
};
}  // namespace Graphics