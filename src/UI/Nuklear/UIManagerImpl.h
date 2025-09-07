#pragma once

#define NUKLEAR_IMPLEMENTATION
#define NUKLEAR_D3D11_IMPLEMENTATION

#include "nuklear.h"
#include "UI/UIManager.h"
#include "nuklear_d3d11.h"

namespace UI {
class UIManagerImpl : public UIManager {
  struct NuklearState {
    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    ID3D11RenderTargetView* renderTargetView;
  };

 public:
  UIManagerImpl() = default;
  ~UIManagerImpl() override = default;

  bool InitializeRenderer(ID3D11Device* device,
                          ID3D11DeviceContext* deviceContext,
                          DXGI_SWAP_CHAIN_DESC& desc) override {
    // Initialize Nuklear with Direct3D 11
    nuklearState_ = {
        .device = device,
        .deviceContext = deviceContext,
    };
    SetSwapChainSize(desc.BufferDesc.Width, desc.BufferDesc.Height);
    ctx_ = nk_d3d11_init(nuklearState_.device, desc.BufferDesc.Width,
                         desc.BufferDesc.Height, UINT16_MAX, UINT16_MAX);
    {
      nk_font_atlas* atlas;
      nk_d3d11_font_stash_begin(&atlas);
      // here's where we load fonts
      nk_d3d11_font_stash_end();
    }

    return true;
  }

  void Update(u32 timer) override {
    ForEachWindow([&](Window* window) { window->Update(timer); });
  }

  void Render() override {
    if (!isReady_) {
      return;
    }

    if (nk_begin(ctx_, "Demo", nk_rect(50, 50, 230, 250),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
      static nk_colorf bg{.r = 30, .g = 30, .b = 30, .a = 255};
      enum { EASY, HARD };
      static int op = EASY;
      static int property = 20;

      nk_layout_row_static(ctx_, 30, 80, 1);
      if (nk_button_label(ctx_, "button")) (void)fprintf(stdout, "button pressed\n");
      nk_layout_row_dynamic(ctx_, 30, 2);
      if (nk_option_label(ctx_, "easy", op == EASY)) op = EASY;
      if (nk_option_label(ctx_, "hard", op == HARD)) op = HARD;
      nk_layout_row_dynamic(ctx_, 22, 1);
      nk_property_int(ctx_, "Compression:", 0, &property, 100, 10, 1);

      nk_layout_row_dynamic(ctx_, 20, 1);
      nk_label(ctx_, "background:", NK_TEXT_LEFT);
      nk_layout_row_dynamic(ctx_, 25, 1);
      if (nk_combo_begin_color(ctx_, nk_rgb_cf(bg),
                               nk_vec2(nk_widget_width(ctx_), 400))) {
        nk_layout_row_dynamic(ctx_, 120, 1);
        bg = nk_color_picker(ctx_, bg, NK_RGBA);
        nk_layout_row_dynamic(ctx_, 25, 1);
        bg.r = nk_propertyf(ctx_, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
        bg.g = nk_propertyf(ctx_, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
        bg.b = nk_propertyf(ctx_, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
        bg.a = nk_propertyf(ctx_, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
        nk_combo_end(ctx_);
      }
    }
  }

  void FocusLost() override {}
  void HandleKey(const RE::CharEvent* event) override {}
  void HandleMouse(const RE::ButtonEvent* event) override {}

 private:
  NuklearState nuklearState_{};
  nk_context* ctx_;

  void SetSwapChainSize(int width, int height) {
    ID3D11Texture2D* backBuffer;
    D3D11_RENDER_TARGET_VIEW_DESC desc;
    HRESULT hr;
    if (nuklearState_.renderTargetView) {
      nuklearState_.renderTargetView->Release();
      nuklearState_.renderTargetView = nullptr;
    }

    nuklearState_.deviceContext->OMSetRenderTargets(0, NULL, NULL);
    hr = nuklearState_.swapChain->ResizeBuffers(0, width, height,
                                                DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
      _ERROR("Failed to resize swap chain buffers");
      return;
    }
    memset(&desc, 0, sizeof(desc));
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = nuklearState_.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                            (void**)&backBuffer);
    assert(SUCCEEDED(hr));

    hr = nuklearState_.device->CreateRenderTargetView(
        backBuffer, &desc, &nuklearState_.renderTargetView);
    backBuffer->Release();
    assert(SUCCEEDED(hr));
  }
};
}  // namespace UI