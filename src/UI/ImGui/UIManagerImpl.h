#pragma once

#include <dinput.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "UI/UIManager.h"

namespace UI {
class UIManagerImpl : public UIManager {
 public:
  UIManagerImpl() = default;
  ~UIManagerImpl() override  = default;
  bool InitializeRenderer(ID3D11Device* device,
                          ID3D11DeviceContext* deviceContext,
                          DXGI_SWAP_CHAIN_DESC& desc) override {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard |
                     ImGuiConfigFlags_NavEnableGamepad |
                     ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr;
    io.FontDefault = io.Fonts->AddFontFromFileTTF(
        "Data/interface/fonts/NotoSansDisplay-Regular.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF(
        "Data/interface/fonts/NotoSansDisplay-Regular.ttf", 18.0f, nullptr,
        io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromFileTTF(
        "Data/interface/fonts/NotoSansDisplay-Regular.ttf", 18.0f, nullptr,
        io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->AddFontFromFileTTF(
        "Data/interface/fonts/NotoSansDisplay-Regular.ttf", 18.0f, nullptr,
        io.Fonts->GetGlyphRangesKorean());
    io.Fonts->AddFontFromFileTTF(
        "Data/interface/fonts/NotoSansDisplay-Regular.ttf", 18.0f, nullptr,
        io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
      _ERROR("ImGui_ImplWin32_Init failed");
      return false;
    }
    if (!ImGui_ImplDX11_Init(device, deviceContext)) {
      _ERROR("ImGui_ImplDX11_Init failed");
      return false;
    }
    return true;
  }

  void Update(u32 timer) override {
    ForEachWindow([&](Window* window) { window->Update(timer); });
  }

  void Render() override {
    if (!isReady_) {
      return;
    }
    auto& io = ImGui::GetIO();
    Update(io.DeltaTime);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
      const auto [width, height] = RE::BSGraphics::Renderer::GetScreenSize();
      io.DisplaySize = ImVec2(width, height);
    }
    inputLock_.lock();
    ImGui::NewFrame();
    inputLock_.unlock();
    auto didDrawMenu = false;
    ForEachWindow([&](const Window* window) {
      if (!window->IsOpen()) {
        return;
      }
      didDrawMenu = true;
      auto windowSize = window->GetWindowSize();
      auto windowPos = window->GetWindowPos();
      ImGui::SetNextWindowSize({windowSize.x, windowSize.y}, ImGuiCond_Once);
      ImGui::SetNextWindowPos({windowPos.x, windowPos.y}, ImGuiCond_Once);
      window->Render();
    });
    if (didDrawMenu) {
      io.MouseDrawCursor = true;
    }
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }

  void FocusLost() override { ImGui::GetIO().ClearInputKeys(); }

  void HandleKey(const RE::CharEvent* event) override {
    if (!event || !isReady_) {
      return;
    }
    auto& io = ImGui::GetIO();
    io.AddInputCharacter(ImGui_ImplWin32_VirtualKeyToImGuiKey(event->keyCode));
  }

  void HandleMouse(const RE::ButtonEvent* event) override {
    auto& io = ImGui::GetIO();
    switch (const auto key =
                static_cast<RE::BSWin32MouseDevice::Key>(event->GetIDCode())) {
      case RE::BSWin32MouseDevice::Key::kWheelUp:
        io.AddMouseWheelEvent(0, event->Value());
        break;
      case RE::BSWin32MouseDevice::Key::kWheelDown:
        io.AddMouseWheelEvent(0, -event->Value());
        break;
      default:
        io.AddMouseButtonEvent(key, event->IsPressed());
        break;
    }
  }

 private:
  static ImVec2 GetWindowSize() {
    static const auto SCREEN_SIZE = RE::BSGraphics::Renderer::GetScreenSize();
    return ImVec2(SCREEN_SIZE.width, SCREEN_SIZE.height);
  }

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

  static std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key key) {
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

#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)
  static ImGuiKey ImGui_ImplWin32_VirtualKeyToImGuiKey(WPARAM wParam) {
    using namespace REX::W32;
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
};
}  // namespace UI