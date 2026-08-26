#pragma once

#include <RmlUi/Core.h>

namespace RmlUi::Backend::SKSE {
class Window {
public:
  virtual ~Window() = 0;

  virtual void SetPosition(const Rml::Vector2i& pos);
  virtual void SetSize(const Rml::Vector2i& size);
  virtual void Show();
  virtual void Hide();
  virtual bool IsShowing() const;
};

void Install();
bool Initialize(const char* identifier, int width, int height);
bool Initialize(const char* identifier, ID3D11Device* device, HWND hwnd, int width, int height);
void Shutdown();
Rml::RenderInterface* GetRenderInterface();
Rml::SystemInterface* GetSystemInterface();
Rml::Context* GetContext();
void BeginFrame();
void PresentFrame();

// helper methods
bool UseFont(const char* name);
}