#include "UI/Pages/MaterialsPage.h"

#include "Helpers.h"
#include "UI/Components/ApplyToNodeComponent.h"
#include "UI/ImGui_Sugar.h"

namespace UI::Pages {
void MaterialsPage(const MaterialsPageProps&) {
  static auto selectedSlot = RE::BIPED_OBJECTS::BIPED_OBJECT::kBody;
  static std::string selectedMaterial;

  ImGui_Child("MaterialsList",
              ImVec2{ImGui::GetContentRegionAvail().x * 0.6f, 0}) {
    ImGui::Text("Armor slot");
    Helpers::VisitArmorSlot(
        [&](RE::BIPED_OBJECTS::BIPED_OBJECT slot, const std::string& name) {
          ImGui_Button(name.c_str()) {
            selectedSlot = slot;
            selectedMaterial.clear();
          }
        });
    if (selectedSlot == RE::BIPED_OBJECTS::BIPED_OBJECT::kNone) {
      ImGui::Text("No slot selected");
      return;
    }
    ImGui::Text("Selected slot: %s", Helpers::GetSlotName(selectedSlot));
    ImGui::Separator();
    auto bipedObject = RE::PlayerCharacter::GetSingleton()
                           ->GetCurrentBiped()
                           ->objects[selectedSlot];

    if (!bipedObject.item) {
      ImGui::Text("No armor item found for the selected slot.");
      return;
    }

    if (selectedSlot != RE::BIPED_OBJECTS::BIPED_OBJECT::kNone) {
      ImGui_Table("MaterialsTable", 1, ImGuiTableFlags_BordersInnerH) {
        MaterialLoader::VisitMaterialFilesForFormID(
            bipedObject.item->GetFormID(),
            [&](const std::string& materialFile) {
              ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{8, 8});
              ImGui::PushStyleColor(ImGuiCol_Header,
                                    ImVec4{0.2f, 0.2f, 0.2f, 1.0f});
              ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                                    ImVec4{0.3f, 0.3f, 0.3f, 1.0f});
              ImGui_Row {
                ImGui_Column {
                  ImGui_Selectable(materialFile.c_str()) {
                    selectedMaterial = materialFile;
                  }
                  if (selectedMaterial == materialFile) {
                    ImGui::SetItemDefaultFocus();
                  }
                }
              }
              ImGui::PopStyleColor(2);
              ImGui::PopStyleVar();
            });
      }
    }
  }
  ImGui::SameLine();
  ImGui_Child("ApplyToNode") {
    Components::ApplyToNodeComponent(
        {.actor = RE::PlayerCharacter::GetSingleton(),
         .material = selectedMaterial.empty() ? nullptr : &selectedMaterial});
  }
}
}  // namespace UI::Pages
