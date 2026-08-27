#pragma once

namespace UI {
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

inline void Install() {
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
}  // namespace UI