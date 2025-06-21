#pragma once

#include <imgui.h>

namespace ImGui::Sugar {
using ScopeEndCallback = void (*)();

// RAII scope guard for ImGui Begin* functions returning bool.
template <bool AlwaysCallEnd>
struct BooleanGuard {
  BooleanGuard(const bool state, const ScopeEndCallback end) noexcept
      : m_state(state), m_end(end) {}

  BooleanGuard(const BooleanGuard<AlwaysCallEnd>&) = delete;
  BooleanGuard(BooleanGuard<AlwaysCallEnd>&&) = delete;
  BooleanGuard<AlwaysCallEnd>& operator=(const BooleanGuard<AlwaysCallEnd>&) =
      delete;  // NOLINT
  BooleanGuard<AlwaysCallEnd>& operator=(BooleanGuard<AlwaysCallEnd>&&) =
      delete;  // NOLINT

  ~BooleanGuard() noexcept;

  operator bool() const& noexcept { return m_state; }  // (Implicit) NOLINT

 private:
  const bool m_state;
  const ScopeEndCallback m_end;
};

template <>
inline BooleanGuard<true>::~BooleanGuard() noexcept {
  m_end();
}

template <>
inline BooleanGuard<false>::~BooleanGuard() noexcept {
  if (m_state) {
    m_end();
  }
}

// For special cases, transform void(*)(int) to void(*)()
inline void PopStyleColor() { ImGui::PopStyleColor(1); };
inline void PopStyleVar() { ImGui::PopStyleVar(1); };
inline void Unindent() { ImGui::Unindent(); }

// Tooltip auto triggered on hover
inline auto BeginTooltip() -> bool {
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    return true;
  }
  return false;
}

}  // namespace ImGui::Sugar

// ----------------------------------------------------------------------------
// [SECTION] Utility macros
// ----------------------------------------------------------------------------

// Portable Expression Statement, calls void function and returns true
#define IMGUI_SUGAR_ES(FN, ...) \
  ([&]() -> bool {              \
    FN(__VA_ARGS__);            \
    return true;                \
  }())
#define IMGUI_SUGAR_ES_0(FN) \
  ([&]() -> bool {           \
    FN();                    \
    return true;             \
  }())

// Concatenating symbols with __LINE__ requires two levels of indirection
#define IMGUI_SUGAR_CONCAT0(A, B) A##B
#define IMGUI_SUGAR_CONCAT1(A, B) IMGUI_SUGAR_CONCAT0(A, B)

// ----------------------------------------------------------------------------
// [SECTION] Generic macros to simplify repetitive declarations
// ----------------------------------------------------------------------------
//
// +----------------------+-------------------+-----------------+---------------------+
// | BEGIN                | END               | ALWAYS          | __VA_ARGS__ |
// +----------------------+-------------------+-----------------+---------------------+
// | Begin*/Push*         | End*/Pop*         | Is call to END  | Begin*/Push* |
// | function name        | function name     | unconditional?  | function
// arguments  |
// +----------------------+-------------------+-----------------+---------------------+

#define IMGUI_SUGAR_SCOPED_BOOL(BEGIN, END, ALWAYS, ...)            \
  if (const ImGui::Sugar::BooleanGuard<ALWAYS> IMGUI_SUGAR_CONCAT1( \
          _ui_scope_guard, __LINE__) = {BEGIN(__VA_ARGS__), &END})

#define IMGUI_SUGAR_SCOPED_BOOL_0(BEGIN, END, ALWAYS)               \
  if (const ImGui::Sugar::BooleanGuard<ALWAYS> IMGUI_SUGAR_CONCAT1( \
          _ui_scope_guard, __LINE__) = {BEGIN(), &END})

#define IMGUI_SUGAR_SCOPED_VOID_N(BEGIN, END, ...)                          \
  if (const ImGui::Sugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1(           \
          _ui_scope_guard, __LINE__) = {IMGUI_SUGAR_ES(BEGIN, __VA_ARGS__), \
                                        &END})

#define IMGUI_SUGAR_SCOPED_VOID_0(BEGIN, END)                     \
  if (const ImGui::Sugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1( \
          _ui_scope_guard, __LINE__) = {IMGUI_SUGAR_ES_0(BEGIN), &END})

#define IMGUI_SUGAR_PARENT_SCOPED_VOID_N(BEGIN, END, ...)     \
  const ImGui::Sugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1( \
      _ui_scope_, __LINE__) = {IMGUI_SUGAR_ES(BEGIN, __VA_ARGS__), &END}

// ---------------------------------------------------------------------------
// [SECTION] ImGui DSL
// ----------------------------------------------------------------------------

#define ImGui_Window(...) \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::Begin, ImGui::End, true, __VA_ARGS__)
#define ImGui_Child(...) \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginChild, ImGui::EndChild, true, __VA_ARGS__)
#define ImGui_ChildFrame(...)                                                 \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginChildFrame, ImGui::EndChildFrame, true, \
                          __VA_ARGS__)
#define ImGui_Combo(...)                                             \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginCombo, ImGui::EndCombo, false, \
                          __VA_ARGS__)
#define ImGui_ListBox(...)                                               \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginListBox, ImGui::EndListBox, false, \
                          __VA_ARGS__)
#define ImGui_Menu(...) \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginMenu, ImGui::EndMenu, false, __VA_ARGS__)
#define ImGui_Popup(...)                                             \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopup, ImGui::EndPopup, false, \
                          __VA_ARGS__)
#define ImGui_PopupModal(...)                                             \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupModal, ImGui::EndPopup, false, \
                          __VA_ARGS__)
#define ImGui_PopupContextItem(...)                                      \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupContextItem, ImGui::EndPopup, \
                          false, __VA_ARGS__)
#define ImGui_PopupContextWindow(...)                                      \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupContextWindow, ImGui::EndPopup, \
                          false, __VA_ARGS__)
#define ImGui_PopupContextVoid(...)                                      \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupContextVoid, ImGui::EndPopup, \
                          false, __VA_ARGS__)
#define ImGui_Table(...)                                             \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginTable, ImGui::EndTable, false, \
                          __VA_ARGS__)
#define ImGui_TabBar(...)                                              \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginTabBar, ImGui::EndTabBar, false, \
                          __VA_ARGS__)
#define ImGui_TreeNode(...) \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNode, ImGui::TreePop, false, __VA_ARGS__)
#define ImGui_TreeNodeEx(...) \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNodeEx, ImGui::TreePop, false, __VA_ARGS__)
#define ImGui_TreeNodeV(...) \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNodeV, ImGui::TreePop, false, __VA_ARGS__)
#define ImGui_TreeNodeExV(...)                                       \
  IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNodeExV, ImGui::TreePop, false, \
                          __VA_ARGS__)

#define ImGui_TooltipOnHover(...)                                          \
  IMGUI_SUGAR_SCOPED_BOOL_0(ImGui::Sugar::BeginTooltip, ImGui::EndTooltip, \
                            false)
#define ImGui_MainMenuBar(...)                                              \
  IMGUI_SUGAR_SCOPED_BOOL_0(ImGui::BeginMainMenuBar, ImGui::EndMainMenuBar, \
                            false)
#define ImGui_MenuBar(...) \
  IMGUI_SUGAR_SCOPED_BOOL_0(ImGui::BeginMenuBar, ImGui::EndMenuBar, false)

#define ImGui_Group \
  IMGUI_SUGAR_SCOPED_VOID_0(ImGui::BeginGroup, ImGui::EndGroup)
#define ImGui_Tooltip \
  IMGUI_SUGAR_SCOPED_VOID_0(ImGui::BeginTooltip, ImGui::EndTooltip)

#define ImGui_Font(...) \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushFont, ImGui::PopFont, __VA_ARGS__)
#define ImGui_AllowKeyboardFocus(...)                      \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushAllowKeyboardFocus, \
                            ImGui::PopAllowKeyboardFocus, __VA_ARGS__)
#define ImGui_ButtonRepeat(...)                                              \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushButtonRepeat, ImGui::PopButtonRepeat, \
                            __VA_ARGS__)
#define ImGui_ItemWidth(...)                                           \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushItemWidth, ImGui::PopItemWidth, \
                            __VA_ARGS__)
#define ImGui_TextWrapPos(...)                                             \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushTextWrapPos, ImGui::PopTextWrapPos, \
                            __VA_ARGS__)
#define ImGui_StyleColor(...)                      \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushStyleColor, \
                            ImGui::Sugar::PopStyleColor, __VA_ARGS__)
#define ImGui_StyleVar(...)                                                 \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushStyleVar, ImGui::Sugar::PopStyleVar, \
                            __VA_ARGS__)
#define ImGui_Indent(...) \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::Indent, ImGui::Sugar::Unindent, __VA_ARGS__)

#define ImGui_MenuItem(...) if (ImGui::MenuItem(__VA_ARGS__))

#define ImGui_Button(...) if (ImGui::Button(__VA_ARGS__))

#define ImGui_Selectable(...) if (ImGui::Selectable(__VA_ARGS__))

#define ImGui_Row ImGui::TableNextRow();

#define ImGui_Column ImGui::TableNextColumn();

#define ImGui_ID(...) \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushID, ImGui::PopID, __VA_ARGS__)

// Layout
#define ImGui_Horizontal(...)                                             \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::BeginHorizontal, ImGui::EndHorizontal, \
                            __VA_ARGS__)

#define ImGui_Vertical(...)                                           \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::BeginVertical, ImGui::EndVertical, \
                            __VA_ARGS__)

#define ImGui_SuspendLayout() \
  IMGUI_SUGAR_SCOPED_VOID_0(ImGui::SuspendLayout, ImGui::ResumeLayout)