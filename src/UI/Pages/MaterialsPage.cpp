#include "UI/Pages/MaterialsPage.h"

#include "Helpers/SkyrimHelpers.h"
#include "MaterialSwapper.h"
#include "Translations.h"
#include "UI/ImGui_Stylus.h"

#undef GetObject

namespace UI::Pages {
void MaterialsPage(const MaterialsPageProps&) {
  auto* target = RE::Console::GetSelectedRef()
                     ? RE::Console::GetSelectedRef().get()
                     : RE::PlayerCharacter::GetSingleton();

  ImGui_Child("MaterialsList") {
    ImGui_TabBar("MaterialsTabBar") {
      if (auto* actor = target->As<RE::Actor>()) {
        ImGui_TabItem("$MSF_InventoryTabHeader"_tr) {
          auto inventory = actor->GetInventory();
          for (auto& [object, data] : inventory) {
            const auto& [count, entry] = data;
            if (object->IsArmor()) {
              ImGui_Button(entry->GetDisplayName()) {
                
              }
            }
          }
        }
      }
      if (target->IsAnimal() || target->IsHumanoid()) {
        ImGui_TabItem("$MSF_ActorTabHeader"_tr) {
          ImGui::Text("$MSF_NotImplemented"_tr);
        }
      }
    }
  }
}
}  // namespace UI::Pages
