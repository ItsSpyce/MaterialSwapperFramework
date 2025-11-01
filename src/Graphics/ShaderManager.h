// original by Acro, stripped of unnecessary parts
#pragma once

#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>

#include "Result.h"

namespace Graphics {
class ShaderManager : public Singleton<ShaderManager> {
  using PixelShader = ComPtr<ID3D11PixelShader>;
  using Blob = ComPtr<ID3DBlob>;
  using Device = ComPtr<ID3D11Device>;
  using Context = ComPtr<ID3D11DeviceContext>;

  class ShaderLockGuard {
   public:
    ShaderLockGuard() { Lock(); }
    ~ShaderLockGuard() { Unlock(); }

    ShaderLockGuard(const ShaderLockGuard&) = delete;
    ShaderLockGuard(ShaderLockGuard&&) = delete;
    ShaderLockGuard& operator=(const ShaderLockGuard&) = delete;
  };

 public:
  ShaderManager() = default;
  ~ShaderManager() = default;

  PixelShader GetPixelShader(const string& name);
  Blob GetPixelShaderBlob(const string& name);

  ID3D11Device* GetDevice() const { return device_.Get(); }
  ID3D11DeviceContext* GetContext() const { return context_.Get(); }

  static void Lock() {
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
        &RE::BSGraphics::Renderer::GetSingleton()->GetLock()));
  }

  static void Unlock() {
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
        &RE::BSGraphics::Renderer::GetSingleton()->GetLock()));
  }
  void Flush();

  Result<> LoadPixelShader(const char* shaderCode,
                           ID3D11PixelShader** pixelShader, Blob& shaderData);
  Result<> LoadPixelShaderFromFile(const string& filePath,
                                   const string& entryPoint,
                                   ID3D11PixelShader** pixelShader,
                                   Blob& shaderData);
  Result<> LoadCompiledPixelShaderFromFile(const string& filePath,
                                           ID3D11PixelShader** pixelShader);

  string GetShaderPath(const string& shaderName, bool compiled);

 private:
  concurrency::concurrent_unordered_map<string, PixelShader> pixelShaders_;
  concurrency::concurrent_unordered_map<string, Blob> pixelShaderBlobs_;
  concurrency::concurrent_vector<string> failedShaders_;
  Device device_;
  Context context_;
};
}  // namespace Graphics