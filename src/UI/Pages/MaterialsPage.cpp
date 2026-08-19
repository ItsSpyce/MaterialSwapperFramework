#include "UI/Pages/MaterialsPage.h"

#include "Helpers/SkyrimHelpers.h"
#include "MaterialSwapper.h"
#include "Translations.h"
#include "UI/ImGui_Stylus.h"

namespace UI::Pages {
void MaterialsPage(const MaterialsPageProps&) {
  static RE::BipedObjectSlot selectedSlot = RE::BipedObjectSlot::kNone;
  static emhash8::HashMap<int32_t, std::vector<MATC>> availableMaterials;
  auto ref = RE::Console::GetSelectedRef();
  auto* actor = ref && ref->As<RE::Actor>()
                    ? ref->As<RE::Actor>()
                    : RE::PlayerCharacter::GetSingleton();

  ImGui_Child("MaterialsList") {
    ImGui_Button(Translations::materialsPageResetButton()) {
      MaterialSwapper::ResetEquippedArmors(actor);
    }

    ImGui_TabBar("MaterialsTabBar") {
      ImGui_TabItem(Translations::materialsPageArmorsTabHeader()) {
        ImGui_Table("ArmorTable", 2, ImGuiTableFlags_BordersInnerH,
                    {ImGui::GetContentRegionAvail().x * .55f, 0.f}) {
          Helpers::VisitEquippedInventoryItems(
              actor, [&](const RE::BipedObjectSlot slot,
                         RE::InventoryEntryData* data) {
                ImGui_Row {
                  ImGui_Column {
                    ImGui_Stylus(ImGui::Stylus::Styles{
                        .framePadding = ImVec2{4.0f, 8.0f},
                        .buttonTextAlign = ImVec2{0.0f, 0.5f},
                        .borderColor = ImVec4{0.5f, 0.5f, 0.5, 1.0f},
                        .buttonColor = selectedSlot == slot
                                           ? ImVec4{0.2f, 0.4f, 0.8f, 1.0f}
                                           : ImVec4{0.f, 0.f, 0.f, 0.f}}) {
                      ImGui_Button(
                          data->GetDisplayName(),
                          ImVec2{ImGui::GetContentRegionAvail().x, 0.0f}) {
                        selectedSlot = slot;
                        availableMaterials.clear();
                        MaterialSwapper::VisitApplicableMaterials(
                            data->object, [](const MATC& material) {
                              auto [it, added] = availableMaterials.try_emplace(
                                  material.layer, std::vector<MATC>());
                              it->second.push_back(material);
                              return RE::BSVisit::BSVisitControl::kContinue;
                            });
                      }
                    }
                  }
                  ImGui_Column {
                    MaterialSwapper::VisitAppliedArmorMaterials(
                        actor, slot, [](const MATC& config) {
                          ImGui::Text("%s", config.name);
                          ImGui::SameLine();
                          ImGui::Text(";");
                          return RE::BSVisit::BSVisitControl::kContinue;
                        });
                  }
                }
                return RE::BSVisit::BSVisitControl::kContinue;
              });
        }
        ImGui::SameLine();

        if (selectedSlot != RE::BipedObjectSlot::kNone) {
          ImGui_Table("MaterialsTable", 1, ImGuiTableFlags_BordersInnerH,
                      {ImGui::GetContentRegionAvail().x * 0.45f, 0.f}) {
            ImGui_Row {
              ImGui_Column {
                ImGui_Stylus(ImGui::Stylus::Styles{
                    .framePadding = ImVec2{4.0f, 8.0f},
                    .buttonTextAlign = ImVec2{0.0f, 0.5f},
                    .borderColor = ImVec4{0.5f, 0.5f, 0.5, 1.0f},
                    .buttonColor = ImVec4{0.f, 0.f, 0.f, 0.f}}) {
                  ImGui_Button("Default",
                               ImVec2{ImGui::GetContentRegionAvail().x, 0.0f}) {
                    MaterialSwapper::ResetEquippedArmor(actor, selectedSlot);
                  }
                }
              }
            }
            MaterialSwapper::VisitApplicableMaterials(
                actor->GetWornArmor(selectedSlot, true),
                [&](const MATC& material) {
                  if (!material.isHidden) {
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
                                  "Click to apply this material to the "
                                  "selected "
                                  "item.");
                            }
                            MaterialSwapper::ApplyArmorMaterial(
                                actor, selectedSlot, material);
                          }
                        }
                      }
                    }
                  }

                  return RE::BSVisit::BSVisitControl::kContinue;
                });
          }
        }
      }
      ImGui_TabItem(Translations::materialsPageWeaponsTabHeader()) {
        ImGui::Text(Translations::notImplemented());
      }
      ImGui_TabItem(Translations::materialsPageActorsTabHeader()) {
        ImGui::Text(Translations::notImplemented());
      }
      ImGui_TabItem(Translations::materialsPagePresetsTabHeader()) {
        ImGui::Text(Translations::notImplemented());
      }
    }
  }
}
}  // namespace UI::Pages
