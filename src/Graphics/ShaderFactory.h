#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Graphics {

class D3DDevice;

class ShaderFile {
 public:
  virtual void* GetBuffer() const = 0;
  virtual size_t GetBufferSize() const = 0;
  virtual const char* GetSourceName() const = 0;
  virtual const char* GetEntryPoint() const = 0;
};

class ShaderFactory {
 public:
  virtual bool CreateVertexShader(
      D3DDevice* device, ShaderFile* sourceFile, ShaderFile* precompiledFile,
      D3D11_INPUT_ELEMENT_DESC* polygonLayout, int numElements,
      Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
      Microsoft::WRL::ComPtr<ID3D11InputLayout>& layout);
  virtual bool CreatePixelShader(
      D3DDevice* device, ShaderFile* sourceFile, ShaderFile* precompiledFile,
      Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader);
  virtual bool CreateGeometryShader(
      D3DDevice* device, ShaderFile* sourceFile, ShaderFile* precompiled,
      Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometryShader);

 private:
  void OutputShaderErrorMessage(Microsoft::WRL::ComPtr<ID3DBlob>& errorMessage,
                                stringstream& output);
};
}  // namespace Graphics