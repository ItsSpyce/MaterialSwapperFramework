#include "UI.h"

#include <dinput.h>

#include "RmlUi/RmlUi_Backend_SKSE.h"

namespace {
struct RenderedWindow {
  UI::Window window;
  Rml::ElementDocument* doc{nullptr};
};
std::vector<RenderedWindow> g_windows;

bool InitializeRml(ID3D11Device* device, const DXGI_SWAP_CHAIN_DESC& sd) {
  if (!RmlUi::Backend::SKSE::Initialize(
          "MaterialSwapperFramework", device, sd.OutputWindow,
          static_cast<int>(sd.BufferDesc.Width),
          static_cast<int>(sd.BufferDesc.Height))) {
    _ERROR("Failed to initialize RmlUi");
    return false;
  }
  Rml::LoadFontFace(
      "Data/interface/MaterialSwapperFramework/NotoSansDisplay-Regular.ttf");
  return true;
}

void CleanupRml() {
  Rml::Shutdown();
  RmlUi::Backend::SKSE::Shutdown();
}

struct InputHook {
  static void thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher,
                    RE::InputEvent* const* events);
  static inline REL::Relocation<decltype(thunk)> func;
  static inline REL::Relocation rel{REL::VariantID(67315, 68617, 0xC519E0)};
  static inline REL::VariantOffset offset{0x7B, 0x7B, 0x81};
};

struct CreateD3D11Hook {
  static void thunk();
  static inline REL::Relocation<decltype(thunk)> func;
  static inline REL::Relocation rel{REL::VariantID(75595, 77226, 0xDC5530)};
  static inline REL::VariantOffset offset{0x9, 0x275, 0x9};
};

struct PresentHook {
  static void thunk(uint32_t timer);
  static inline REL::Relocation<decltype(thunk)> func;
  static inline REL::Relocation rel{REL::VariantID(75461, 77246, 0xDBBDD0)};
  static inline REL::VariantOffset offset{0x9, 0x9, 0x15};
};

struct Renderer_KillWindow {
  static void thunk(std::uint32_t windowID);
  static inline REL::Relocation<decltype(thunk)> func{
      RELOCATION_ID(75452, 77237)};
};

struct Renderer_Shutdown {
  static void thunk();
  static inline REL::Relocation<decltype(thunk)> func{
      RELOCATION_ID(75447, 77228)};
};

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
  if (!InitializeRml(device, sd)) {
    _WARN("Rml failed to initialize. No interface will be available");
  }
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
}  // namespace

namespace UI {
void Install() {
  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // attempting to catch everything that shuts down the game :shrug:
  stl::write_detour<Renderer_KillWindow>();
  stl::write_detour<Renderer_Shutdown>();
  DetourTransactionCommit();

  stl::write_thunk_call<InputHook>();
  stl::write_thunk_call<CreateD3D11Hook>();
  stl::write_thunk_call<PresentHook>();
}

Window* NewWindow(const char* name, const char* url, const WindowOpts& opts) {
  auto& refr = g_windows.emplace_back(RenderedWindow{
      .window = UI::Window{name, opts.size, opts.pos, opts.flags}});
  WindowShown += [refr = &refr, url, opts](Window* win) {
    if (refr->window == win) {
      if (!refr->doc) {
        auto* ctx = RmlUi::Backend::SKSE::GetContext();
        refr->doc = ctx->LoadDocument(url);
        Box box;
        box.SetContent(opts.size);
        refr->doc->SetBox(box);
      }
      if (win->HasFlag(WindowFlags_AlwaysVisible)) {
        refr->doc->Show();
      }
    }
  };
  WindowHidden += [refr](Window* win) {
    if (refr.window == win && refr.doc) {
      refr.doc->Hide();
    }
  };
  return &refr.window;
}
}  // namespace UI