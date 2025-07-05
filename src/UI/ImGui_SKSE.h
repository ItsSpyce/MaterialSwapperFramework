#pragma once

#include <WinUser.h>
#include <Windows.h>
#include <d3d11.h>
#include <dinput.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

namespace ImGui::SKSE {
#ifdef SKYRIM_AE
#define RELOCATION_OFFSET(se, ae) ae
#else
#define RELOCATION_OFFSET(se, ae) se
#endif
inline static REL::RelocationID kD3DInitHookID = RELOCATION_ID(75595, 77226);
inline constexpr int kD3DInitHookOffset = RELOCATION_OFFSET(0x9, 0x275);
inline static REL::RelocationID kDXGIPresentHookID =
    RELOCATION_ID(75461, 77246);
inline constexpr int kDXGIPresentHookOffset = 0x9;

inline ImVec2 GetWindowSize() {
  static const auto screen_size = RE::BSGraphics::Renderer::GetScreenSize();
  return ImVec2(screen_size.width, screen_size.height);
}

enum class InitStatus : UINT8 {
  kSuccess = 0,
  kFailedWin32Init,
  kFailedDX11Init,
  kFailedSwapchainLocate,
  kFailedSwapchainDesc,
  kFailedDeviceLocate,
  kFailedDeviceContextLocate,
  kNoRenderer,
};
inline InitStatus Install(ID3D11Device* device,
                          ID3D11DeviceContext* device_context,
                          const DXGI_SWAP_CHAIN_DESC& desc) {
  CreateContext();
  auto& io = GetIO();
  io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard |
                   ImGuiConfigFlags_NavEnableGamepad |
                   ImGuiConfigFlags_NoMouseCursorChange;
  io.ConfigWindowsMoveFromTitleBarOnly = true;
  io.IniFilename = nullptr;
  if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
    return InitStatus::kFailedWin32Init;
  }
  if (!ImGui_ImplDX11_Init(device, device_context)) {
    return InitStatus::kFailedDX11Init;
  }
  return InitStatus::kSuccess;
}

inline InitStatus Install(DXGI_SWAP_CHAIN_DESC& sd) {
  const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
  if (!renderer) {
    return InitStatus::kNoRenderer;
  }
  const auto swapchain = (IDXGISwapChain*)renderer->data.renderWindows[0].swapChain;
  if (!swapchain) {
    return InitStatus::kFailedSwapchainLocate;
  }
  if (swapchain->GetDesc(std::addressof(sd)) < 0) {
    return InitStatus::kFailedSwapchainDesc;
  }
  const auto device = (ID3D11Device*)renderer->data.forwarder;
  if (!device) {
    return InitStatus::kFailedDeviceLocate;
  }
  const auto device_context = (ID3D11DeviceContext*)renderer->data.context;
  if (!device_context) {
    return InitStatus::kFailedDeviceContextLocate;
  }
  return Install(device, device_context, sd);
}

#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)
inline ImGuiKey ImGui_ImplWin32_VirtualKeyToImGuiKey(WPARAM wParam) {
  switch (wParam) {
    case VK_TAB:
      return ImGuiKey_Tab;
    case VK_LEFT:
      return ImGuiKey_LeftArrow;
    case VK_RIGHT:
      return ImGuiKey_RightArrow;
    case VK_UP:
      return ImGuiKey_UpArrow;
    case VK_DOWN:
      return ImGuiKey_DownArrow;
    case VK_PRIOR:
      return ImGuiKey_PageUp;
    case VK_NEXT:
      return ImGuiKey_PageDown;
    case VK_HOME:
      return ImGuiKey_Home;
    case VK_END:
      return ImGuiKey_End;
    case VK_INSERT:
      return ImGuiKey_Insert;
    case VK_DELETE:
      return ImGuiKey_Delete;
    case VK_BACK:
      return ImGuiKey_Backspace;
    case VK_SPACE:
      return ImGuiKey_Space;
    case VK_RETURN:
      return ImGuiKey_Enter;
    case VK_ESCAPE:
      return ImGuiKey_Escape;
    case VK_OEM_7:
      return ImGuiKey_Apostrophe;
    case VK_OEM_COMMA:
      return ImGuiKey_Comma;
    case VK_OEM_MINUS:
      return ImGuiKey_Minus;
    case VK_OEM_PERIOD:
      return ImGuiKey_Period;
    case VK_OEM_2:
      return ImGuiKey_Slash;
    case VK_OEM_1:
      return ImGuiKey_Semicolon;
    case VK_OEM_PLUS:
      return ImGuiKey_Equal;
    case VK_OEM_4:
      return ImGuiKey_LeftBracket;
    case VK_OEM_5:
      return ImGuiKey_Backslash;
    case VK_OEM_6:
      return ImGuiKey_RightBracket;
    case VK_OEM_3:
      return ImGuiKey_GraveAccent;
    case VK_CAPITAL:
      return ImGuiKey_CapsLock;
    case VK_SCROLL:
      return ImGuiKey_ScrollLock;
    case VK_NUMLOCK:
      return ImGuiKey_NumLock;
    case VK_SNAPSHOT:
      return ImGuiKey_PrintScreen;
    case VK_PAUSE:
      return ImGuiKey_Pause;
    case VK_NUMPAD0:
      return ImGuiKey_Keypad0;
    case VK_NUMPAD1:
      return ImGuiKey_Keypad1;
    case VK_NUMPAD2:
      return ImGuiKey_Keypad2;
    case VK_NUMPAD3:
      return ImGuiKey_Keypad3;
    case VK_NUMPAD4:
      return ImGuiKey_Keypad4;
    case VK_NUMPAD5:
      return ImGuiKey_Keypad5;
    case VK_NUMPAD6:
      return ImGuiKey_Keypad6;
    case VK_NUMPAD7:
      return ImGuiKey_Keypad7;
    case VK_NUMPAD8:
      return ImGuiKey_Keypad8;
    case VK_NUMPAD9:
      return ImGuiKey_Keypad9;
    case VK_DECIMAL:
      return ImGuiKey_KeypadDecimal;
    case VK_DIVIDE:
      return ImGuiKey_KeypadDivide;
    case VK_MULTIPLY:
      return ImGuiKey_KeypadMultiply;
    case VK_SUBTRACT:
      return ImGuiKey_KeypadSubtract;
    case VK_ADD:
      return ImGuiKey_KeypadAdd;
    case IM_VK_KEYPAD_ENTER:
      return ImGuiKey_KeypadEnter;
    case VK_LSHIFT:
      return ImGuiKey_LeftShift;
    case VK_LCONTROL:
      return ImGuiKey_LeftCtrl;
    case VK_LMENU:
      return ImGuiKey_LeftAlt;
    case VK_LWIN:
      return ImGuiKey_LeftSuper;
    case VK_RSHIFT:
      return ImGuiKey_RightShift;
    case VK_RCONTROL:
      return ImGuiKey_RightCtrl;
    case VK_RMENU:
      return ImGuiKey_RightAlt;
    case VK_RWIN:
      return ImGuiKey_RightSuper;
    case VK_APPS:
      return ImGuiKey_Menu;
    case '0':
      return ImGuiKey_0;
    case '1':
      return ImGuiKey_1;
    case '2':
      return ImGuiKey_2;
    case '3':
      return ImGuiKey_3;
    case '4':
      return ImGuiKey_4;
    case '5':
      return ImGuiKey_5;
    case '6':
      return ImGuiKey_6;
    case '7':
      return ImGuiKey_7;
    case '8':
      return ImGuiKey_8;
    case '9':
      return ImGuiKey_9;
    case 'A':
      return ImGuiKey_A;
    case 'B':
      return ImGuiKey_B;
    case 'C':
      return ImGuiKey_C;
    case 'D':
      return ImGuiKey_D;
    case 'E':
      return ImGuiKey_E;
    case 'F':
      return ImGuiKey_F;
    case 'G':
      return ImGuiKey_G;
    case 'H':
      return ImGuiKey_H;
    case 'I':
      return ImGuiKey_I;
    case 'J':
      return ImGuiKey_J;
    case 'K':
      return ImGuiKey_K;
    case 'L':
      return ImGuiKey_L;
    case 'M':
      return ImGuiKey_M;
    case 'N':
      return ImGuiKey_N;
    case 'O':
      return ImGuiKey_O;
    case 'P':
      return ImGuiKey_P;
    case 'Q':
      return ImGuiKey_Q;
    case 'R':
      return ImGuiKey_R;
    case 'S':
      return ImGuiKey_S;
    case 'T':
      return ImGuiKey_T;
    case 'U':
      return ImGuiKey_U;
    case 'V':
      return ImGuiKey_V;
    case 'W':
      return ImGuiKey_W;
    case 'X':
      return ImGuiKey_X;
    case 'Y':
      return ImGuiKey_Y;
    case 'Z':
      return ImGuiKey_Z;
    case VK_F1:
      return ImGuiKey_F1;
    case VK_F2:
      return ImGuiKey_F2;
    case VK_F3:
      return ImGuiKey_F3;
    case VK_F4:
      return ImGuiKey_F4;
    case VK_F5:
      return ImGuiKey_F5;
    case VK_F6:
      return ImGuiKey_F6;
    case VK_F7:
      return ImGuiKey_F7;
    case VK_F8:
      return ImGuiKey_F8;
    case VK_F9:
      return ImGuiKey_F9;
    case VK_F10:
      return ImGuiKey_F10;
    case VK_F11:
      return ImGuiKey_F11;
    case VK_F12:
      return ImGuiKey_F12;
    default:
      return ImGuiKey_None;
  }
}

inline UINT ConvertDirectInputKeyToVirtualKey(uint32_t key) {
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
      return IM_VK_KEYPAD_ENTER;
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

template <bool (*IsActive)(), bool PauseTimeInMenus>
class WindowManager {
 public:
  enum WindowType : uint8_t {
    WindowType_kNone,
    WindowType_kWidget,
    WindowType_kMenu,
  };

  class UIWindow {
   public:
    UIWindow() = default;
    virtual ~UIWindow() = default;

    virtual void Initialize() = 0;
    virtual void Update(uint32_t timer) = 0;
    virtual void Draw() = 0;
    virtual bool IsEnabled() const = 0;
    virtual bool IsOpen() const { return is_open_; }
    virtual void SetOpen(bool open) { is_open_ = open; }
    virtual bool IsShowKey(RE::InputEvent* event) = 0;

    virtual constexpr uint8_t GetWindowType() = 0;

    void ToggleVisibility() { is_open_ = !is_open_; }
    ImVec2 GetWindowSize() const { return window_size_; }
    ImVec2 GetWindowPos() const { return window_pos_; }
    void SetWindowSize(const ImVec2& size) { window_size_ = size; }
    void SetWindowPos(const ImVec2& pos) { window_pos_ = pos; }

   protected:
    std::atomic_bool is_open_{false};
    ImVec2 window_size_{};
    ImVec2 window_pos_{};
  };

  template <class Window>
  static void RegisterWindow() {
    windowsLock_.lock();
    windows_[windowsCount_++] = new Window();
    windowsLock_.unlock();
  }

 private:
  struct WndProcHook {
    static LRESULT thunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
      auto& io = GetIO();
      if (msg == WM_KILLFOCUS) {
        io.ClearInputKeys();
      }
      return func(hwnd, msg, wparam, lparam);
    }
    static inline WNDPROC func;
  };

  struct D3DInitHook {
    static void thunk() {
      func();
      InitImpl();
    }
    static inline REL::Relocation<decltype(thunk)> func;
  };

  struct DXGIPresentHook {
    static void thunk(uint32_t timer) {
      func(timer);
      if (!IsActive()) {
        return;
      }
      ForEachWindow([timer](UIWindow* window) {
        if (window->IsEnabled()) {
          window->Update(timer);
        }
      });
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      {
        static const auto window_size = GetWindowSize();
        auto& io = GetIO();
        io.DisplaySize = window_size;
      }
      inputLock_.lock();
      NewFrame();
      inputLock_.unlock();

      auto& io = GetIO();
      auto didDrawMenu = false;
      ForEachWindow([&](UIWindow* window) {
        if (window->IsOpen()) {
          if (window->GetWindowType() == WindowType_kMenu) {
            didDrawMenu = true;
          }
          window->Draw();
        }
      });
      if (didDrawMenu) {
        io.MouseDrawCursor = true;
      }

      EndFrame();
      Render();
      ImGui_ImplDX11_RenderDrawData(GetDrawData());
      if (!RE::Main::GetSingleton()->freezeTime) {
        RE::Main::GetSingleton()->freezeTime = didDrawMenu && PauseTimeInMenus;
      }
    }
    static inline REL::Relocation<decltype(thunk)> func;
  };

  struct ProcessInputQueueHook {
    static void thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher,
                      RE::InputEvent* const* events) {
      UpdateCursor();
      static RE::InputEvent* dummy_events = nullptr;
      if (ShouldGrabInput(events)) {
        func(dispatcher, &dummy_events);
        inputLock_.lock();
        HandleInputQueue(events);
        inputLock_.unlock();
      } else {
        func(dispatcher, events);
      }
    }
    static inline REL::Relocation<decltype(thunk)> func;
  };

  class UIControlSink : public RE::BSTEventSink<RE::MenuModeChangeEvent> {
   public:
    static UIControlSink* GetSingleton() {
      static UIControlSink singleton;
      return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(
        const RE::MenuModeChangeEvent* a_event,
        RE::BSTEventSource<RE::MenuModeChangeEvent>* a_dispatcher) override {
      if (a_event->mode == RE::MenuModeChangeEvent::Mode::kDisplayed) {
        canShowWindows_.store(false);
      } else {
        canShowWindows_.store(true);
      }
      return RE::BSEventNotifyControl::kContinue;
    }
  };

 public:
  static void RegisterHooks() {
    logger::info("Registering hooks");
    RegisterHooksImpl();
  }

  static void Initialize() {
    logger::info("Initializing window manager");
    ForEachWindow([](UIWindow* window) { window->Initialize(); });
  }
  static void ToggleOpen() {
    for (UIWindow* window : windows_) {
      if (!window) {
        continue;
      }
      if (window->GetWindowType() == WindowType_kMenu) {
        window->ToggleVisibility();
      }
    }
  }

 private:
  static void InitImpl() {
    DXGI_SWAP_CHAIN_DESC sd;
    switch (Install(sd)) {
      case InitStatus::kFailedWin32Init:
        logger::error("ImGui_ImplWin32_Init failed");
        return;
      case InitStatus::kFailedDX11Init:
        logger::error("ImGui_ImplDX11_Init failed");
        return;
      case InitStatus::kFailedSwapchainLocate:
        logger::error("IDXGISwapChain failed");
        return;
      case InitStatus::kFailedSwapchainDesc:
        logger::error("IDXGISwapChain::GetDesc failed");
        return;
      case InitStatus::kFailedDeviceLocate:
        logger::error("Failed to locate device");
        return;
      case InitStatus::kFailedDeviceContextLocate:
        logger::error("Failed to locate device context");
        return;
      case InitStatus::kNoRenderer:
        logger::error("No renderer found");
        return;
      case InitStatus::kSuccess:
        break;
    }

    WndProcHook::func = (WNDPROC)SetWindowLongPtrA(
        sd.OutputWindow, GWLP_WNDPROC, (LONG_PTR)WndProcHook::thunk);
    if (!WndProcHook::func) {
      logger::error("SetWindowLongPtrA failed");
    }
  }

  static void RegisterHooksImpl() {
    const REL::Relocation d3dInitHook{kD3DInitHookID,
                                       kD3DInitHookOffset};  // BSGraphics::InitD3D
    stl::write_thunk_call<D3DInitHook>(d3dInitHook.address());
    const REL::Relocation dxgiHook{
        kDXGIPresentHookID,
        kDXGIPresentHookOffset};  // BSGraphics::Renderer::End
    stl::write_thunk_call<DXGIPresentHook>(dxgiHook.address());
    const REL::Relocation processInputQueueHook{RELOCATION_ID(67315, 68617), 0x7B};
    stl::write_thunk_call<ProcessInputQueueHook>(
        processInputQueueHook.address());

    RE::UI::GetSingleton()->AddEventSink<RE::MenuModeChangeEvent>(
        UIControlSink::GetSingleton());
  }

  static bool ShouldGrabInput(RE::InputEvent* const* events) {
    for (auto event = *events; event; event = event->next) {
      for (auto& window : windows_) {
        if (!window) {
          continue;
        }
        if (window->IsShowKey(event)) {
          window->ToggleVisibility();
          return window->GetWindowType() == WindowType_kMenu;
        }
        if (window->IsOpen() && window->GetWindowType() == WindowType_kMenu) {
          return true;
        }
      }
    }
    return false;
  }

  static void HandleInputQueue(RE::InputEvent* const* events) {
    auto& io = GetIO();
    for (auto event = *events; event; event = event->next) {
      if (const auto charEvent = event->AsCharEvent()) {
        io.AddInputCharacter(charEvent->keycode);
        continue;
      }
      if (const auto buttonEvent = event->AsButtonEvent()) {
        if (!buttonEvent->HasIDCode()) {
          continue;
        }
        if (buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
          const auto imKey =
              ImGui_ImplWin32_VirtualKeyToImGuiKey(buttonEvent->GetIDCode());
          const auto pressed = buttonEvent->IsPressed();
          io.AddKeyEvent(imKey, pressed);
        }
        if (buttonEvent->GetDevice() == RE::INPUT_DEVICE::kMouse) {
          switch (const auto key = static_cast<RE::BSWin32MouseDevice::Key>(
                      buttonEvent->GetIDCode())) {
            case RE::BSWin32MouseDevice::Key::kWheelUp:
              io.AddMouseWheelEvent(0, buttonEvent->Value());
              break;
            case RE::BSWin32MouseDevice::Key::kWheelDown:
              io.AddMouseWheelEvent(0, buttonEvent->Value() * -1);
              break;
            default:
              io.AddMouseButtonEvent(key, buttonEvent->IsPressed());
              break;
          }
        }
      }
    }
  }

  static void UpdateCursor() {
    auto& io = GetIO();
    io.MouseDrawCursor = drawCursor_;
  }

  static void ForEachWindow(std::function<void(UIWindow*)> func) {
    windowsLock_.lock();
    for (unsigned i = 0; i < windowsCount_; ++i) {
      func(windows_[i]);
    }
    windowsLock_.unlock();
  }

  static const UIWindow** GetOpenMenus() {
    windowsLock_.lock();
    auto windows =
        std::find_if(windows_, [](UIWindow* window) {
          return window && window->IsOpen() &&
                 window->GetWindowType() == WindowType_kMenu;
        });
    windowsLock_.unlock();
    return windows;
  }

  static const UIWindow** GetOpenWidgets() {
    windowsLock_.lock();
    auto widgets =
        std::find_if(windows_, [](UIWindow* window) {
          return window && window->IsOpen() &&
                 window->GetWindowType() == WindowType_kWidget;
        });
    windowsLock_.unlock();
    return widgets;
  }

  static inline std::mutex inputLock_;
  static inline std::mutex windowsLock_;
  static inline UIWindow* windows_[16];
  static inline uint8_t windowsCount_ = 0;
  static inline std::atomic_bool drawCursor_{false};
  static inline std::atomic_bool canShowWindows_{true};
};

class CharEvent : public RE::InputEvent {
 public:
  uint32_t key_code;  // 18 (ascii code)
};

enum : std::uint32_t {
  kInvalid = static_cast<std::uint32_t>(-1),
  kKeyboardOffset = 0,
  kMouseOffset = 256,
  kGamepadOffset = 266
};

inline std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key key) {
  using Key = RE::BSWin32GamepadDevice::Key;

  std::uint32_t index;
  switch (key) {
    case Key::kUp:
      index = 0;
      break;
    case Key::kDown:
      index = 1;
      break;
    case Key::kLeft:
      index = 2;
      break;
    case Key::kRight:
      index = 3;
      break;
    case Key::kStart:
      index = 4;
      break;
    case Key::kBack:
      index = 5;
      break;
    case Key::kLeftThumb:
      index = 6;
      break;
    case Key::kRightThumb:
      index = 7;
      break;
    case Key::kLeftShoulder:
      index = 8;
      break;
    case Key::kRightShoulder:
      index = 9;
      break;
    case Key::kA:
      index = 10;
      break;
    case Key::kB:
      index = 11;
      break;
    case Key::kX:
      index = 12;
      break;
    case Key::kY:
      index = 13;
      break;
    case Key::kLeftTrigger:
      index = 14;
      break;
    case Key::kRightTrigger:
      index = 15;
      break;
    case Key::kLeftStick:
      index = 16;
      break;
    case Key::kRightStick:
      index = 17;
      break;
    default:
      index = kInvalid;
      break;
  }

  return index != kInvalid ? index + kGamepadOffset : kInvalid;
}
}  // namespace ImGui::SKSE