#pragma once

#include "Hooks.h"

namespace UI {
class Window {
 public:
  virtual void Initialize() {}
  virtual void Render() const = 0;
  virtual void Update(u32 timer) = 0;
  void Show() { isOpen_ = true; }
  void Hide() { isOpen_ = false; }
  NODISCARD bool IsOpen() const { return isOpen_; }
  NODISCARD Vector2 GetWindowSize() const { return windowSize_; }
  NODISCARD Vector2 GetWindowPos() const { return windowPos_; }
  void SetWindowSize(const Vector2& size) { windowSize_ = size; }
  void SetWindowPos(const Vector2& pos) { windowPos_ = pos; }

 private:
  bool isOpen_{true};
  Vector2 windowSize_{400, 300};
  Vector2 windowPos_{100, 100};
};

class UIManager {
  class UIControlSink : public RE::BSTEventSink<RE::MenuModeChangeEvent>,
                        public Singleton<UIControlSink> {
   public:
    RE::BSEventNotifyControl ProcessEvent(
        const RE::MenuModeChangeEvent* a_event,
        RE::BSTEventSource<RE::MenuModeChangeEvent>*) override {
      if (a_event->mode == RE::MenuModeChangeEvent::Mode::kDisplayed) {
        isShowing_ = false;
      }
      return RE::BSEventNotifyControl::kContinue;
    }
  };

 public:
  UIManager() = default;
  virtual ~UIManager() = default;
  UIManager(UIManager&&) = delete;
  UIManager(const UIManager&) = delete;
  UIManager& operator=(UIManager&&) = delete;
  UIManager& operator=(const UIManager&) = delete;

  virtual bool InitializeRenderer(ID3D11Device* device,
                                  ID3D11DeviceContext* deviceContext,
                                  DXGI_SWAP_CHAIN_DESC& desc) = 0;
  virtual void FocusLost() = 0;
  virtual void HandleKey(const RE::CharEvent* event) = 0;
  virtual void HandleMouse(const RE::ButtonEvent* event) = 0;
  virtual void Render() = 0;
  virtual void Update(u32 timer) = 0;

  bool Install(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
               DXGI_SWAP_CHAIN_DESC& desc) {
    if (InitializeRenderer(device, deviceContext, desc)) {
      Hooks::WndProcHandlerHook::func = (WNDPROC)SetWindowLongPtrA(
          desc.OutputWindow, GWLP_WNDPROC,
          (LONG_PTR)Hooks::WndProcHandlerHook::thunk);
      return true;
    }
    return false;
  }

  void Initialize() {
    if (isReady_) {
      return;
    }
    isReady_ = true;
    RE::UI::GetSingleton()->AddEventSink<RE::MenuModeChangeEvent>(
        UIControlSink::GetSingleton());
    ForEachWindow([](Window* window) { window->Initialize(); });
  }

  static bool IsShowKey(const RE::ButtonEvent* event) {
    return event->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
           (event->GetIDCode() == RE::BSWin32KeyboardDevice::Keys::kF10 ||
            (event->GetIDCode() == RE::BSWin32KeyboardDevice::Keys::kEscape &&
             isShowing_));
  }

  static bool ShouldGrabInput(RE::InputEvent* const* events) {
    if (!isReady_) {
      return false;
    }
    for (auto event = *events; event; event = event->next) {
      if (const auto buttonEvent = event->AsButtonEvent()) {
        if (buttonEvent->IsDown() && IsShowKey(buttonEvent)) {
          isShowing_ = !isShowing_;
          return true;
        }
      }
    }
    return isShowing_;
  }

  void HandleInputQueue(RE::InputEvent* const* events) {
    for (auto event = *events; event; event = event->next) {
      if (const auto charEvent = event->AsCharEvent()) {
        HandleKey(charEvent);
        continue;
      }
      if (const auto buttonEvent = event->AsButtonEvent()) {
        if (!buttonEvent->HasIDCode()) {
          continue;
        }
        if (buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
          const auto vk =
              ConvertDirectInputKeyToVirtualKey(buttonEvent->GetIDCode());
          if (vk != 0 && buttonEvent->IsPressed()) {
            HandleKey(buttonEvent->AsCharEvent());
            continue;
          }
        }
        if (buttonEvent->GetDevice() == RE::INPUT_DEVICE::kMouse) {
          HandleMouse(buttonEvent);
        }
      }
    }
  }

  void ProcessInputQueue(RE::InputEvent* const* events) {
    if (!isReady_) {
      return;
    }
    inputLock_.lock();
    HandleInputQueue(events);
    inputLock_.unlock();
  }
  void AddWindow(Window* window) {
    std::scoped_lock lock(windowsLock_);
    if (windowsCount_ < windows_.size()) {
      windows_[windowsCount_++] = window;
    }
  }

  void RemoveWindow(Window* window) {
    std::scoped_lock lock(windowsLock_);
    for (u8 i = 0; i < windowsCount_; ++i) {
      if (windows_[i] == window) {
        for (u8 j = i; j < windowsCount_ - 1; ++j) {
          windows_[j] = windows_[j + 1];
        }
        windows_[--windowsCount_] = nullptr;
        break;
      }
    }
  }

  void ToggleVisibility() { isShowing_ = !isShowing_; }
  void SetVisibility(bool show) { isShowing_ = show; }
  bool IsVisible() const { return isShowing_; }
  bool IsDrawingCursor() const { return drawCursor_; }
  void SetDrawCursor(bool draw) { drawCursor_ = draw; }

 protected:
  static inline bool isReady_;
  static inline bool isShowing_;
  static inline bool drawCursor_;
  static inline std::mutex windowsLock_;
  static inline array<Window*, 16> windows_;
  static inline u8 windowsCount_;
  std::mutex inputLock_;
  static UINT ConvertDirectInputKeyToVirtualKey(uint32_t key) {
    using namespace REX::W32;
    switch (key) {
      case DIK_LEFTARROW:
        return VK_LEFT;
      case DIK_RIGHTARROW:
        return VK_RIGHT;
      case DIK_UPARROW:
        return VK_UP;
      case DIK_DOWNARROW:
        return VK_DOWN;
      case DIK_DELETE:
        return VK_DELETE;
      case DIK_END:
        return VK_END;
      case DIK_HOME:
        return VK_HOME;
      case DIK_PRIOR:
        return VK_PRIOR;
      case DIK_NEXT:
        return VK_NEXT;
      case DIK_INSERT:
        return VK_INSERT;
      case DIK_NUMPAD0:
        return VK_NUMPAD0;
      case DIK_NUMPAD1:
        return VK_NUMPAD1;
      case DIK_NUMPAD2:
        return VK_NUMPAD2;
      case DIK_NUMPAD3:
        return VK_NUMPAD3;
      case DIK_NUMPAD4:
        return VK_NUMPAD4;
      case DIK_NUMPAD5:
        return VK_NUMPAD5;
      case DIK_NUMPAD6:
        return VK_NUMPAD6;
      case DIK_NUMPAD7:
        return VK_NUMPAD7;
      case DIK_NUMPAD8:
        return VK_NUMPAD8;
      case DIK_NUMPAD9:
        return VK_NUMPAD9;
      case DIK_DECIMAL:
        return VK_DECIMAL;
      case DIK_NUMPADENTER:
        return VK_RETURN;
      case DIK_RMENU:
        return VK_RMENU;
      case DIK_RCONTROL:
        return VK_RCONTROL;
      case DIK_LWIN:
        return VK_LWIN;
      case DIK_RWIN:
        return VK_RWIN;
      case DIK_APPS:
        return VK_APPS;
      default:
        return MapVirtualKeyEx(key, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
    }
  }

  void ForEachWindow(const function<void(Window*)>& func) {
    std::scoped_lock lock(windowsLock_);
    for (u8 i = 0; i < windowsCount_; ++i) {
      if (auto* window = windows_[i]) {
        func(windows_[i]);
      }
    }
  }
};
}  // namespace UI