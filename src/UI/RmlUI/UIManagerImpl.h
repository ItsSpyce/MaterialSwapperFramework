#pragma once

#include <RmlUI/Core.h>

#include <efsw/efsw.hpp>

#include "RmlUi/RmlUi_Backend_SKSE.h"
#include "UI/UIManager.h"

namespace UI {
class UIManagerImpl : public UIManager {
  Rml::ElementDocument* doc_{nullptr};

  class InterfaceFilesystemListener : public efsw::FileWatchListener {
   public:
    void handleFileAction(efsw::WatchID watchid, const std::string& dir,
                          const std::string& filename, efsw::Action action,
                          const std::string& oldFilename) override {
      //
    }
  };

 public:
  UIManagerImpl() = default;
  ~UIManagerImpl() override = default;

  bool InitializeRenderer(ID3D11Device* device,
                          ID3D11DeviceContext* deviceContext,
                          DXGI_SWAP_CHAIN_DESC& desc) override {
    if (!RmlUi::Backend::SKSE::Initialize(
            "MSF", device, desc.OutputWindow,
            static_cast<int>(desc.BufferDesc.Width),
            static_cast<int>(desc.BufferDesc.Height))) {
      return false;
    }
    if (!Rml::LoadFontFace("Data/interface/fonts/NotoSansDisplay-Regular.ttf")) {
      _ERROR("Failed to load font");
      return false;
    }
    doc_ = RmlUi::Backend::SKSE::GetContext()->LoadDocument(
        "Data/interface/MaterialSwapperFramework/index.rml");

    if (!doc_) {
      _ERROR("Failed to load Rml document");
      return false;
    }
    doc_->Show();
    return true;
  }

  void Update(float timer) override {}

  void Render() override {
    auto* ctx = RmlUi::Backend::SKSE::GetContext();
    RmlUi::Backend::SKSE::BeginFrame();
    ctx->Render();
    RmlUi::Backend::SKSE::PresentFrame();
  }

  void FocusLost() override {}
  void HandleKey(const RE::CharEvent* event) override {}
  void HandleMouse(const RE::ButtonEvent* event) override {}
};
}  // namespace UI