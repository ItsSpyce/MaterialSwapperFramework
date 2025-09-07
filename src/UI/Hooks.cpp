#include "UI/Hooks.h"

#include "UI.h"
#include "UIManager.h"

void UI::Hooks::InputHook::thunk(
    RE::BSTEventSource<RE::InputEvent*>* dispatcher,
    RE::InputEvent* const* events) {
  if (UIManager::ShouldGrabInput(events)) {
    constexpr RE::InputEvent* const dummyEvents[] = {nullptr};
    auto* ui = GetCurrentUI();
    func(dispatcher, dummyEvents);
    ui->ProcessInputQueue(events);
  } else {
    func(dispatcher, events);
  }
}

LRESULT UI::Hooks::WndProcHandlerHook::thunk(HWND hwnd, UINT msg, WPARAM wParam,
                                             LPARAM lParam) {
  GetCurrentUI()->FocusLost();
  return func(hwnd, msg, wParam, lParam);
}

void UI::Hooks::CreateD3D11Hook::thunk() {
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
  auto* context = reinterpret_cast<ID3D11DeviceContext*>(
      renderer->GetRuntimeData().context);
  if (!context) {
    _ERROR("Failed to locate device context");
    return;
  }
  if (!GetCurrentUI()->Install(device, context, sd)) {
    _ERROR("Failed to initialize UI");
  }
}

void UI::Hooks::PresentHook::thunk(uint32_t timer) {
  func(timer);
  if (auto* ui = GetCurrentUI(); ui && ui->IsVisible()) {
    ui->Render();
  }
}
