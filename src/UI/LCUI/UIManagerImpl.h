#pragma once

#include "UI/UIManager.h"

namespace UI {
class UIManagerImpl : public UIManager {
 public:
  UIManagerImpl() = default;
  ~UIManagerImpl() override = default;

  bool InitializeRenderer(ID3D11Device* device,
                          ID3D11DeviceContext* deviceContext,
                          DXGI_SWAP_CHAIN_DESC& desc) override {
    // LCUI_Init();
    return true;
  }

  void Update(u32 timer) override {
    ForEachWindow([&](Window* window) { window->Update(timer); });
  }

  void Render() override {
    //
  }

  void FocusLost() override {}

  void HandleKey(const RE::CharEvent* event) override {}

  void HandleMouse(const RE::ButtonEvent* event) override {}

 private:
  ID3D11Texture2D* renderTarget = nullptr;
  ID3D11RenderTargetView* renderTargetView = nullptr;
};
}  // namespace UI