#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Graphics {
class D3DDevice {
 public:
  D3DDevice() : device_(nullptr), context_(nullptr) {}
  D3DDevice(const Microsoft::WRL::ComPtr<ID3D11Device>& device,
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
      : device_(device), context_(context) {}

  void SetDevice(const Microsoft::WRL::ComPtr<ID3D11Device>& device) {
    device_ = device;
  }

  void SetContext(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) {
    context_ = context;
  }

  Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const { return device_; }
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const {
    return context_;
  }

 private:
  Microsoft::WRL::ComPtr<ID3D11Device> device_;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
};
}  // namespace Graphics