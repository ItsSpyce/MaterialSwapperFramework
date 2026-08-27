#include "UI.h"

#include <dinput.h>

#include "RmlUi/RmlUi_Backend_SKSE.h"

namespace {
Rml::ElementDocument* g_doc = nullptr;

void CleanupRml() {
  Rml::Shutdown();
  RmlUi::Backend::SKSE::Shutdown();
}
}  // namespace

namespace UI {
void InputHook::thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher,
                      RE::InputEvent* const* events) {
  static constexpr RE::InputEvent* dummyEvents[] = {nullptr};
  static bool isInputEnabled = false;
  auto* ctx = RmlUi::Backend::SKSE::GetContext();
  if (!isInputEnabled) {
    for (auto event = *events; event; event = event->next) {
      if (const auto* buttonEvent = event->AsButtonEvent()) {
        if (ctx && buttonEvent->IsDown() &&
            buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
            buttonEvent->GetIDCode() == DIK_F10) {
          isInputEnabled = true;
          func(dispatcher, dummyEvents);
          ctx->EnableMouseCursor(true);
          return;
        }
      }
    }
    func(dispatcher, events);
  } else {
    for (auto event = *events; event; event = event->next) {
      if (const auto* buttonEvent = event->AsButtonEvent()) {
        if (ctx && buttonEvent->IsDown() &&
            buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
            (buttonEvent->GetIDCode() == DIK_F10 ||
             buttonEvent->GetIDCode() == DIK_ESCAPE)) {
          isInputEnabled = false;
          ctx->EnableMouseCursor(false);
        }
        /*if (buttonEvent->IsDown()) {
          switch (event->GetDevice()) {
            case RE::INPUT_DEVICE::kKeyboard:
              g_ctx->ProcessKeyDown(Rml::Input::KI_A,)
              break;
            case RE::INPUT_DEVICE::kMouse:
              break;
            default:
              continue;
          }
        } else if (buttonEvent->IsUp()) {
          if (buttonEvent->GetDevice() == RE::INPUT_DEVICE::kMouse) {

          }
        }*/
      }
    }
  }
}

void CreateD3D11Hook::thunk() {
  func();
  _DEBUG("CreateD3D11Hook called");
  const auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
  if (!renderer) {
    _ERROR("Failed to locate renderer");
    return;
  }
  auto* swapchain = reinterpret_cast<IDXGISwapChain*>(
      renderer->GetRuntimeData().renderWindows[0].swapChain);
  if (!swapchain) {
    _ERROR("Failed to locate swapchain");
    return;
  }
  DXGI_SWAP_CHAIN_DESC sd;
  if (swapchain->GetDesc(std::addressof(sd)) < 0) {
    _ERROR("Failed to get swapchain description");
    return;
  }
  auto* device =
      reinterpret_cast<ID3D11Device*>(renderer->GetRuntimeData().forwarder);
  if (!device) {
    _ERROR("Failed to locate device");
    return;
  }
  if (!RmlUi::Backend::SKSE::Initialize(
          "MaterialSwapperFramework", device, sd.OutputWindow,
          static_cast<int>(sd.BufferDesc.Width),
          static_cast<int>(sd.BufferDesc.Height))) {
    _ERROR("Failed to initialize RmlUi");
    return;
  }
  Rml::LoadFontFace(
      "Data/interface/MaterialSwapperFramework/NotoSansDisplay-Regular.ttf");
  auto* ctx = RmlUi::Backend::SKSE::GetContext();
  if (g_doc = ctx->LoadDocument(
          "Data/interface/MaterialSwapperFramework/index.rml");
      !g_doc) {
    _ERROR("Failed to load document");
    return;
  }
  g_doc->Show();
}

void PresentHook::thunk(uint32_t timer) {
  func(timer);
  if (auto* ctx = RmlUi::Backend::SKSE::GetContext()) {
    RmlUi::Backend::SKSE::BeginFrame();
    ctx->Render();
    RmlUi::Backend::SKSE::PresentFrame();
  }
}

void Renderer_KillWindow::thunk(std::uint32_t windowID) {
  CleanupRml();
  func(windowID);
}

void Renderer_Shutdown::thunk() {
  CleanupRml();
  func();
}
}  // namespace UI