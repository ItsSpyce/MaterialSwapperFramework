#include "UI/Pages/MaterialsPage.h"

#include "Factories/ArmorFactory.h"
#include "Helpers.h"
#include "IO/MaterialLoader.h"
#include "Models/MaterialConfig.h"
#include "UI/ImGui_Stylus.h"

namespace UI::Pages {
void MaterialsPage(const MaterialsPageProps&) {
  static RE::TESObjectARMO* selectedArmor;
  auto ref = RE::Console::GetSelectedRef();
  auto* actor = ref ? ref.get() : RE::PlayerCharacter::GetSingleton();

  ImGui_Child("MaterialsList") {
    ImGui_Button("Reset Materials") {
      Factories::ArmorFactory::GetSingleton()->ResetMaterials(actor);
    }

    ImGui_TabBar("MaterialsTabBar") {
      ImGui_TabItem("Armor") {
        ImGui_Table("ArmorTable", 2, ImGuiTableFlags_BordersInnerH,
                    {ImGui::GetContentRegionAvail().x * .55f, 0.f}) {
          Helpers::VisitEquippedInventoryItems(
              actor, [&](const Helpers::InventoryItem* invItem) {
                if (auto armo =
                        invItem->data->object->As<RE::TESObjectARMO>()) {
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
                    ImGui_Column {
                      Factories::ArmorFactory::GetSingleton()
                          ->VisitAppliedMaterials(
                              invItem->uid,
                              [&](const char* materialName,
                                  const MaterialConfig&) {
                                ImGui::Text("%s", materialName);
                                ImGui::SameLine();
                                ImGui::Text(";");
                                return RE::BSVisit::BSVisitControl::kContinue;
                              });
                    }
                  }
                }
                return RE::BSVisit::BSVisitControl::kContinue;
              });
        }
        ImGui::SameLine();

        if (selectedArmor) {
          ImGui_Table("MaterialsTable", 1, ImGuiTableFlags_BordersInnerH,
                      {ImGui::GetContentRegionAvail().x * 0.45f, 0.f}) {
            MaterialLoader::VisitMaterialFilesForFormID(
                selectedArmor->GetFormID(),
                [&](const MaterialConfig& material) {
                  if (material.isHidden) {
                    return RE::BSVisit::BSVisitControl::kContinue;
                  }
                  ImGui_Row {
                    ImGui_Column {
                      ImGui_Stylus(ImGui::Stylus::Styles{
                          .framePadding = ImVec2{4.0f, 8.0f},
                          .buttonTextAlign = ImVec2{0.0f, 0.5f},
                          .borderColor = ImVec4{0.5f, 0.5f, 0.5, 1.0f},
                          .buttonColor = ImVec4{0.f, 0.f, 0.f, 0.f}}) {
                        ImGui_Button(
                            material.name.c_str(),
                            ImVec2{ImGui::GetContentRegionAvail().x, 0.0f}) {
                          if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip(
                                "Click to apply this material to the selected "
                                "item.");
                          }
                          Factories::ArmorFactory::GetSingleton()
                              ->ApplyMaterial(actor, selectedArmor, &material);
                        }
                      }
                    }
                  }

                  return RE::BSVisit::BSVisitControl::kContinue;
                });
          }
        }
      }
      ImGui_TabItem("Weapons") { ImGui::Text("Weapon materials coming soon!"); }
      ImGui_TabItem("Actors") {
        //
      }
    }
  }
}
}  // namespace UI::Pages
