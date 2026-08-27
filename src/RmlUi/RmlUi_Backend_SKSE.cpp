#include "RmlUi_Backend_SKSE.h"

#include "RmlUi_Platform_Win32.h"
#include "RmlUi_Renderer_DX11.h"
#include "Translations.h"

namespace {

RenderInterface_DX11* g_renderInterface;
RmlUi::Backend::SKSE::SystemInterface_SKSE* g_systemInterface;
RmlUi::Backend::SKSE::FileInterface_SKSE* g_fileInterface;
Rml::Context* g_ctx;

}  // namespace

namespace RmlUi::Backend::SKSE {

namespace logger = ::SKSE::log;
class SystemInterface_SKSE : public SystemInterface_Win32 {
 public:
  SystemInterface_SKSE() {
    SetWindow((HWND)RE::BSGraphics::Renderer::GetCurrentRenderWindow()->hWnd);
  }

  int TranslateString(Rml::String& translated,
                      const Rml::String& input) override {
    // return 1 if translated, 0 if not
    return Translations::GetTranslation(input.c_str()).match<int>(
      [&](const std::string& out) {
        translated = out;
        return 1;
      },
      [&](const std::string& err) {
        _WARN("Failed to translate string: {}", err);
        translated = input;
        return 0;
      }
    );
  }

  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
    switch (type) {
      case Rml::Log::Type::LT_INFO:
        logger::info("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_WARNING:
        logger::warn("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_ERROR:
        logger::error("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_DEBUG:
        logger::debug("RmlUi - {}", message);
        return true;
      case Rml::Log::Type::LT_ASSERT:
        logger::trace("RmlUi - {}", message);
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

void Shutdown() {
  //
}

RenderInterface_DX11* GetRenderInterface() { return g_renderInterface; }

SystemInterface_SKSE* GetSystemInterface() { return g_systemInterface; }

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

bool LoadDocument(const Rml::String& documentPath) { return false; }
}  // namespace RmlUi::Backend::SKSE