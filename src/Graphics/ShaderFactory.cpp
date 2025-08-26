#include "ShaderFactory.h"

#include <sstream>

#include "D3DDevice.h"
#include "ShaderCompile.h"

bool Graphics::ShaderFactory::CreatePixelShader(
    D3DDevice* device, ShaderFile* sourceFile, ShaderFile* precompiledFile,
    Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader) {
  Microsoft::WRL::ComPtr<ID3D11Device> pDevice = device->GetDevice();
  Microsoft::WRL::ComPtr<ID3DBlob> errorMessage;
  Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

  // Compile the pixel shader
  HRESULT result = S_OK;
  if (precompiledFile) {
    result = CompileShaderFromData(
        precompiledFile->GetBuffer(), precompiledFile->GetBufferSize(),
        precompiledFile->GetSourceName(), precompiledFile->GetEntryPoint(),
        "ps_5_0", shaderBlob.GetAddressOf(), errorMessage.GetAddressOf());
  } else if (sourceFile) {
    result = CompileShaderFromData(
        sourceFile->GetBuffer(), sourceFile->GetBufferSize(),
        sourceFile->GetSourceName(), sourceFile->GetEntryPoint(), "ps_5_0",
        shaderBlob.GetAddressOf(), errorMessage.GetAddressOf());
  } else {
    return false;
  }
  if (FAILED(result)) {
    if (errorMessage) {
      std::stringstream ss;
      OutputShaderErrorMessage(errorMessage, ss);
      logger::error("Pixel Shader Compilation Error: {}", ss.str());
    } else {
      logger::error("Failed to compile pixel shader from file: {}",
                    sourceFile ? sourceFile->GetSourceName() : "unknown");
    }
    return false;
  }
  // Create the pixel shader
  result = pDevice->CreatePixelShader(shaderBlob->GetBufferPointer(),
                                      shaderBlob->GetBufferSize(), nullptr,
                                      pixelShader.ReleaseAndGetAddressOf());
  if (FAILED(result)) {
    logger::error("Failed to create pixel shader from file: {}",
                  sourceFile ? sourceFile->GetSourceName() : "unknown");
  }
  return SUCCEEDED(result);
}

bool Graphics::ShaderFactory::CreateVertexShader(
    D3DDevice* device, ShaderFile* sourceFile, ShaderFile* precompiledFile,
    D3D11_INPUT_ELEMENT_DESC* polygonLayout, int numElements,
    Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
    Microsoft::WRL::ComPtr<ID3D11InputLayout>& layout) {
  Microsoft::WRL::ComPtr<ID3D11Device> pDevice = device->GetDevice();
  Microsoft::WRL::ComPtr<ID3DBlob> errorMessage;
  Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

  stringstream errors;

  HRESULT result = S_OK;
  if (precompiledFile) {
    result = CompileShaderFromData(
        precompiledFile->GetBuffer(), precompiledFile->GetBufferSize(),
        precompiledFile->GetSourceName(), precompiledFile->GetEntryPoint(),
        "vs_5_0", shaderBlob.GetAddressOf(), errorMessage.GetAddressOf());
  } else if (sourceFile) {
    result = CompileShaderFromData(
        sourceFile->GetBuffer(), sourceFile->GetBufferSize(),
        sourceFile->GetSourceName(), sourceFile->GetEntryPoint(), "vs_5_0",
        shaderBlob.GetAddressOf(), errorMessage.GetAddressOf());
  } else {
    return false;
  }
  if (FAILED(result)) {
    if (errorMessage) {
      OutputShaderErrorMessage(errorMessage, errors);
      logger::error("Vertex Shader Compilation Error: {}", errors.str());
    } else {
      logger::error("Failed to compile vertex shader from file: {}",
                    sourceFile ? sourceFile->GetSourceName() : "unknown");
    }
    return false;
  }
  // Create the vertex shader
  result = pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(),
                                       shaderBlob->GetBufferSize(), nullptr,
                                       vertexShader.ReleaseAndGetAddressOf());
  if (FAILED(result)) {
    logger::error("Failed to create vertex shader from file: {}",
                  sourceFile ? sourceFile->GetSourceName() : "unknown");
  }
  return SUCCEEDED(result);
}

bool Graphics::ShaderFactory::CreateGeometryShader(
    D3DDevice* device, ShaderFile* sourceFile, ShaderFile* precompiledFile,
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometryShader) {
  Microsoft::WRL::ComPtr<ID3D11Device> pDevice = device->GetDevice();
  Microsoft::WRL::ComPtr<ID3DBlob> errorMessage;
  Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
  HRESULT result = S_OK;
  if (precompiledFile) {
    result = CompileShaderFromData(
        precompiledFile->GetBuffer(), precompiledFile->GetBufferSize(),
        precompiledFile->GetSourceName(), precompiledFile->GetEntryPoint(),
        "gs_5_0", shaderBlob.GetAddressOf(), errorMessage.GetAddressOf());
  } else if (sourceFile) {
    result = CompileShaderFromData(
        sourceFile->GetBuffer(), sourceFile->GetBufferSize(),
        sourceFile->GetSourceName(), sourceFile->GetEntryPoint(), "gs_5_0",
        shaderBlob.GetAddressOf(), errorMessage.ReleaseAndGetAddressOf());
  } else {
    return false;
  }
  if (FAILED(result)) {
    if (errorMessage) {
      std::stringstream ss;
      OutputShaderErrorMessage(errorMessage, ss);
      logger::error("Geometry Shader Compilation Error: {}", ss.str());
    } else {
      logger::error("Failed to compile geometry shader from file: {}",
                    sourceFile ? sourceFile->GetSourceName() : "unknown");
    }
    return false;
  }
  // Create the geometry shader
  result = pDevice->CreateGeometryShader(
      shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
      geometryShader.ReleaseAndGetAddressOf());
  if (FAILED(result)) {
    logger::error("Failed to create geometry shader from file: {}",
                  sourceFile ? sourceFile->GetSourceName() : "unknown");
  }
  return SUCCEEDED(result);
}

void Graphics::ShaderFactory::OutputShaderErrorMessage(Microsoft::WRL::ComPtr<ID3DBlob>& errorMessage, stringstream& output) {
  char* compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());
  size_t bufferSize = errorMessage->GetBufferSize();
  if (bufferSize > 0) {
    output << "Shader Compilation Error: " << std::string(compileErrors, bufferSize);
  } else {
    output << "Unknown shader compilation error.";
  }
  if (output.str().empty()) {
    output << "No error message provided.";
  }
  if (output.str().back() != '\n') {
    output << '\n';
  }
  logger::error("Shader Compilation Error: {}", output.str());
  if (errorMessage) {
    errorMessage->Release();
  }
  errorMessage = nullptr;
}

