#include "RmlUi_Backend_SKSE.h"

#include <dinput.h>

#include "RmlUi_Platform_Win32.h"
#include "RmlUi_Renderer_DX11.h"
#include "Translations.h"

namespace logger = SKSE::log;

namespace {
class SystemInterface_SKSE : public SystemInterface_Win32 {
 public:
  SystemInterface_SKSE() {
    SetWindow((HWND)RE::BSGraphics::Renderer::GetCurrentRenderWindow()->hWnd);
  }

  int TranslateString(Rml::String& translated,
                      const Rml::String& input) override {
    // return 1 if translated, 0 if not
    if (auto tr = Translations::GetTranslation(input.c_str())) {
      translated = tr.value();
      return 1;
    }
    translated = input;
    return 0;
  }

  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
    switch (type) {
      case Rml::Log::Type::LT_INFO:
        SKSE::log::info("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_WARNING:
        SKSE::log::warn("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_ERROR:
        SKSE::log::error("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_DEBUG:
        SKSE::log::debug("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_ASSERT:
        SKSE::log::trace("RmlUi - {}", message);
        return true;
      default:
        return false;
    }
  }
};

class FileInterface_SKSE : public Rml::FileInterface {
 public:
  FileInterface_SKSE(const char* rootDir)
      : rootDir_(std::filesystem::canonical(
            std::filesystem::path("Data/interface") / rootDir)) {}

  constexpr std::filesystem::path GetRootDir() const { return rootDir_; }

  Rml::FileHandle Open(const Rml::String& path) override {
    const auto resolved = std::filesystem::canonical(path);
    if (const auto it = std::search(resolved.begin(), resolved.end(),
                                    rootDir_.begin(), rootDir_.end());
        it == resolved.begin()) {
      return (Rml::FileHandle)fopen(resolved.string().c_str(), "rb");
    }
    logger::error("Attempted to load RmlUi resource outside of current root");
    return NULL;
  }

  void Close(Rml::FileHandle file) override { fclose((FILE*)file); }

  size_t Read(void* buffer, size_t size, Rml::FileHandle file) override {
    return fread(buffer, 1, size, (FILE*)file);
  }

  bool Seek(Rml::FileHandle file, long offset, int origin) override {
    return fseek((FILE*)file, offset, origin) == 0;
  }

  size_t Tell(Rml::FileHandle file) override { return ftell((FILE*)file); }

 private:
  std::filesystem::path rootDir_;
};

RenderInterface_DX11* g_renderInterface;
SystemInterface_SKSE* g_systemInterface;
FileInterface_SKSE* g_fileInterface;
Rml::Context* g_ctx;
bool g_isInputEnabled = false;

struct InputHook {
  static void thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher,
                    RE::InputEvent* const* events) {
    static constexpr RE::InputEvent* dummyEvents[] = {nullptr};
    if (!g_isInputEnabled) {
      for (auto event = *events; event; event = event->next) {
        if (const auto* buttonEvent = event->AsButtonEvent()) {
          if (g_ctx && buttonEvent->IsDown() &&
              buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
              buttonEvent->GetIDCode() == DIK_F10) {
            g_isInputEnabled = true;
            func(dispatcher, dummyEvents);
            g_ctx->EnableMouseCursor(true);
            return;
          }
        }
      }
      func(dispatcher, events);
    } else {
      for (auto event = *events; event; event = event->next) {
        if (const auto* buttonEvent = event->AsButtonEvent()) {
          if (g_ctx && buttonEvent->IsDown() &&
              buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
              (buttonEvent->GetIDCode() == DIK_F10 ||
               buttonEvent->GetIDCode() == DIK_ESCAPE)) {
            g_isInputEnabled = false;
            g_ctx->EnableMouseCursor(false);
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
  static inline REL::Relocation<decltype(thunk)> func;
  static inline REL::Relocation rel{REL::VariantID(67315, 68617, 0xC519E0)};
  static inline REL::VariantOffset offset{0x7B, 0x7B, 0x81};
};

struct CreateD3D11Hook {
  static void thunk() {
    func();
    logger::debug("CreateD3D11Hook called");
    const auto* renderer = RE::BSGraphics::Renderer::GetSingleton();
    if (!renderer) {
      logger::error("Failed to locate renderer");
      return;
    }
    auto* swapchain = reinterpret_cast<IDXGISwapChain*>(
        renderer->GetRuntimeData().renderWindows[0].swapChain);
    if (!swapchain) {
      logger::error("Failed to locate swapchain");
      return;
    }
    DXGI_SWAP_CHAIN_DESC sd;
    if (swapchain->GetDesc(std::addressof(sd)) < 0) {
      logger::error("Failed to get swapchain description");
      return;
    }
    auto* device =
        reinterpret_cast<ID3D11Device*>(renderer->GetRuntimeData().forwarder);
    if (!device) {
      logger::error("Failed to locate device");
      return;
    }
    if (!RmlUi::Backend::SKSE::Initialize("MaterialSwapperFramework", device,
                                     sd.OutputWindow,
                                     static_cast<int>(sd.BufferDesc.Width),
                                     static_cast<int>(sd.BufferDesc.Height))) {
      logger::error("Failed to initialize RmlUi");
    }
  }
  static inline REL::Relocation<decltype(thunk)> func;
  static inline REL::Relocation rel{REL::VariantID(75595, 77226, 0xDC5530)};
  static inline REL::VariantOffset offset{0x9, 0x275, 0x9};
};

struct PresentHook {
  static void thunk(uint32_t timer) {
    func(timer);
    if (g_ctx) {
      RmlUi::Backend::SKSE::BeginFrame();
      // TODO:
      RmlUi::Backend::SKSE::PresentFrame();
    }
  }
  static inline REL::Relocation<decltype(thunk)> func;
  static inline REL::Relocation rel{REL::VariantID(75461, 77246, 0xDBBDD0)};
  static inline REL::VariantOffset offset{0x9, 0x9, 0x15};
};
}  // namespace

namespace RmlUi::Backend::SKSE {
void Install() {
  stl::write_thunk_call<InputHook>();
  stl::write_thunk_call<CreateD3D11Hook>();
  stl::write_thunk_call<PresentHook>();
}

bool Initialize(const char* identifier, int width, int height) {
  return Initialize(
      identifier, (ID3D11Device*)RE::BSGraphics::Renderer::GetDevice(),
      (HWND)RE::BSGraphics::Renderer::GetCurrentRenderWindow()->hWnd, width,
      height);
}

bool Initialize(const char* identifier, ID3D11Device* device, HWND hwnd,
                int width, int height) {
  g_renderInterface = new RenderInterface_DX11(device);
  g_systemInterface = new SystemInterface_SKSE();
  g_fileInterface = new FileInterface_SKSE(identifier);
  Rml::SetRenderInterface(g_renderInterface);
  Rml::SetSystemInterface(g_systemInterface);
  Rml::SetFileInterface(g_fileInterface);
  if (!Rml::Initialise()) {
    // not using logging macros since it's kinda supposed to be shared
    logger::error("Failed to initialize RmlUi");
    return false;
  }
  g_renderInterface->SetViewport(width, height);
  if (g_ctx = Rml::CreateContext(identifier, Rml::Vector2i{width, height});
      !g_ctx) {
    logger::error("Failed to initialize RmlUi context");
    return false;
  }
  return true;
}

void Shutdown() {}

Rml::RenderInterface* GetRenderInterface() { return g_renderInterface; }

Rml::SystemInterface* GetSystemInterface() { return g_systemInterface; }

Rml::Context* GetContext() { return g_ctx; }

void BeginFrame() {
  if (!g_ctx) return;
  g_ctx->Update();
  g_renderInterface->BeginFrame();
}

void PresentFrame() {
  auto* rtv = (ID3D11RenderTargetView*)
                  RE::BSGraphics::Renderer::GetCurrentRenderWindow()
                      ->renderView;
  g_renderInterface->EndFrame(rtv);
}
}  // namespace RmlUi::Backend::SKSE