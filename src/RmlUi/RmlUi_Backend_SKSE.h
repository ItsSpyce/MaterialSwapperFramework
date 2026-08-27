#pragma once

#include <RmlUi/Core.h>

class RenderInterface_DX11;

namespace RmlUi::Backend::SKSE {
class SystemInterface_SKSE;
class FileInterface_SKSE;

bool Initialize(const char* identifier, int width, int height);
bool Initialize(const char* identifier, ID3D11Device* device, HWND hwnd, int width, int height);
void Shutdown();
RenderInterface_DX11* GetRenderInterface();
SystemInterface_SKSE* GetSystemInterface();
Rml::Context* GetContext();
void BeginFrame();
void PresentFrame();
bool LoadDocument(const Rml::String& documentPath);

// helper methods
}