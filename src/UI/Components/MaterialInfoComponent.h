#pragma once

#include "MaterialConfig.h"
#include "MaterialWriter.h"
#include "UI/ImGui_Sugar.h"

namespace UI::Components {
struct MaterialInfoComponentProps {
  MaterialRecord material;
  RE::BIPED_OBJECTS::BIPED_OBJECT selectedSlot;
};

inline void MaterialInfoComponent(const MaterialInfoComponentProps& props) {
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4.0f, 8.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2{0.0f, 0.5f});
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{0.5f, 0.5f, 0.5f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.f, 0.f, 0.f, 0.f});

  ImGui_Button(props.material.name.c_str(),
               ImVec2{ImGui::GetContentRegionAvail().x, 0.0f}) {
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Click to apply this material to the selected item.");
    }
    MaterialWriter::ApplyMaterial(RE::PlayerCharacter::GetSingleton(),
                                  props.selectedSlot, props.material);
  }

  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar(2);
}
}  // namespace MaterialSwapperFramework::UI::Components