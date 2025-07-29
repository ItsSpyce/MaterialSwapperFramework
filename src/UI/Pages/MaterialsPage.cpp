#include "UI/Pages/MaterialsPage.h"

#include "Factories/ArmorFactory.h"
#include "Helpers.h"
#include "UI/ImGui_Stylus.h"

namespace UI::Pages {
void MaterialsPage(const MaterialsPageProps&) {
  static RE::TESObjectARMO* selectedArmor;

  ImGui_Child("MaterialsList") {
    ImGui_Button("Reset Materials") {
      Factories::ArmorFactory::ResetMaterials(
          RE::PlayerCharacter::GetSingleton());
    }

    ImGui_Table("ArmorTable", 1, ImGuiTableFlags_BordersInnerH,
                {ImGui::GetContentRegionAvail().x * 0.25f, 0.f}) {
      Helpers::VisitEquippedInventoryItems(
          RE::PlayerCharacter::GetSingleton(),
          [&](const std::unique_ptr<Helpers::InventoryItem>& invItem) {
            if (auto armo = invItem->data->object->As<RE::TESObjectARMO>()) {
              ImGui_Row {
                ImGui_Column {
                  ImGui_Stylus(ImGui::Stylus::Styles{
                      .framePadding = ImVec2{4.0f, 8.0f},
                      .buttonTextAlign = ImVec2{0.0f, 0.5f},
                      .borderColor = ImVec4{0.5f, 0.5f, 0.5, 1.0f},
                      .buttonColor =
                          selectedArmor && selectedArmor->GetFormID() ==
                                               armo->GetFormID()
                              ? ImVec4{0.2f, 0.4f, 0.8f, 1.0f}
                              : ImVec4{0.f, 0.f, 0.f, 0.f}}) {
                    ImGui_Button(
                        armo->GetFullName(),
                        ImVec2{ImGui::GetContentRegionAvail().x, 0.0f}) {
                      selectedArmor = armo;
                    }
                  }
                }
              }
            }
          });
    }
    ImGui::SameLine();

    if (selectedArmor) {
      ImGui_Table("MaterialsTable", 1, ImGuiTableFlags_BordersInnerH,
                  {ImGui::GetContentRegionAvail().x * 0.75f, 0.f}) {
        MaterialLoader::VisitMaterialFilesForFormID(
            selectedArmor->GetFormID(),
            [&](const std::unique_ptr<MaterialConfig>& material) {
              if (material->isHidden) {
                return;  // Skip isHidden materials
              }
              ImGui_Row {
                ImGui_Column {
                  ImGui_Stylus(ImGui::Stylus::Styles{
                      .framePadding = ImVec2{4.0f, 8.0f},
                      .buttonTextAlign = ImVec2{0.0f, 0.5f},
                      .borderColor = ImVec4{0.5f, 0.5f, 0.5, 1.0f},
                      .buttonColor = ImVec4{0.f, 0.f, 0.f, 0.f}}) {
                    ImGui_Button(
                        material->name.c_str(),
                        ImVec2{ImGui::GetContentRegionAvail().x, 0.0f}) {
                      if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip(
                            "Click to apply this material to the selected "
                            "item.");
                      }
                      Factories::ArmorFactory::GetSingleton()->ApplyMaterial(
                          RE::PlayerCharacter::GetSingleton(), selectedArmor,
                          std::move(material));
                    }
                  }
                }
              }
            });
      }
    }
  }
}
}  // namespace UI::Pages
