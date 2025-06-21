#include "UI/Pages/MaterialsPage.h"

#include "MaterialLoader.h"
#include "MaterialWriter.h"
#include "StringHelpers.h"
#include "UI/Components/ApplyToNodeComponent.h"
#include "UI/Components/MaterialInfoComponent.h"
#include "UI/ImGui_Sugar.h"

namespace UI::Pages {
void MaterialsPage(const MaterialsPageProps&) {
  static auto selectedSlot = RE::BIPED_OBJECTS::BIPED_OBJECT::kBody;

  ImGui_Child("MaterialsList") {
    ImGui_Combo("Armor slot", StringHelpers::GetSlotName(selectedSlot)) {
      Helpers::VisitArmorSlot(
          [&](RE::BIPED_OBJECTS::BIPED_OBJECT slot, const std::string& name) {
            if (ImGui::Selectable(name.c_str(), selectedSlot == slot)) {
              selectedSlot = slot;
            }
          });
    }
    if (selectedSlot == RE::BIPED_OBJECTS::BIPED_OBJECT::kNone) {
      ImGui::Text("No slot selected");
      return;
    }
    ImGui::Text("Selected slot: %s", StringHelpers::GetSlotName(selectedSlot));
    ImGui::Separator();
    auto& bipedObject = RE::PlayerCharacter::GetSingleton()
                            ->GetCurrentBiped()
                            ->objects[selectedSlot];

    if (!bipedObject.item) {
      ImGui::Text("No armor item found for the selected slot.");
      return;
    }

    if (selectedSlot != RE::BIPED_OBJECTS::BIPED_OBJECT::kNone) {
      ImGui_Table("MaterialsTable", 1, ImGuiTableFlags_BordersInnerH) {
        MaterialLoader::VisitMaterialFilesForFormID(
            bipedObject.item->GetFormID(), [&](const MaterialRecord& material) {
              if (material.isHidden) {
                return;  // Skip isHidden materials
              }
              ImGui_Row {
                ImGui_Column {
                  Components::MaterialInfoComponent(
                      {.material = material, .selectedSlot = selectedSlot});
                }
              }
            });
      }
    }
  }
}
}  // namespace UI::Pages
