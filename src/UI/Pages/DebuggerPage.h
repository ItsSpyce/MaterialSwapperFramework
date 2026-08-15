#pragma once

#include "MaterialSwapper.h"
#include "UI/ImGui_Sugar.h"

namespace UI::Pages {
struct DebuggerPageProps {
  // Add any properties needed for the debugger page
  RE::Actor* actor = nullptr;
};

inline void DebuggerPage(const DebuggerPageProps& props) {
  // This function will render the debugger page
  ImGui::Text("Debugger Page");
  ImGui::Separator();
  ImGui_Child("HelperButtons", ImVec2{ImGui::GetContentRegionAvail().x / 2, 0}) {
    ImGui_Button("Clear save data") {
      MaterialSwapper::ResetAll();
    }
  }
  // Add more debugging features here as needed
};
}  // namespace UI::Pages