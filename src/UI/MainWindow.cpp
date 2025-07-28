#include "UI.h"
#include "UI/Pages/MaterialsPage.h"
#include "UI/Pages/SettingsPage.h"
#include "UI/Router.h"

namespace UI {

void MainWindow::Initialize() {
  auto* router = Router::GetSingleton();
  router->UseRoute("/", [] { Pages::MaterialsPage({}); });
  router->UseRoute("/settings", [] { Pages::SettingsPage({}); });
}

void MainWindow::Draw() {
  ImGui::SetNextWindowSize(ImVec2{1500, 600}, ImGuiCond_Always);
  ImGui_Window("MaterialSwapperFramework", NULL, ImGuiWindowFlags_NoTitleBar) {
    ImGui::Text("Material Swapper Framework");
    ImGui::Separator();
    ImGui::Text("Press F10 to toggle this window.");
    ImGui::Separator();
    ImGui_Child("Sidenav", ImVec2{150, 0}) {
      ImGui::Text("Navigation");
      ImGui::Separator();
      Link({.path = "/", .label = "Home"});
      Link({.path = "/settings", .label = "Settings"});
    }
    ImGui::SameLine();
    ImGui_Child("Content") { Router::GetSingleton()->operator()(); }
  }
}

bool MainWindow::IsShowKey(RE::InputEvent* event) {
  if (const auto buttonEvent = event->AsButtonEvent()) {
    return buttonEvent->IsDown() &&
           buttonEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
           (buttonEvent->GetIDCode() == RE::BSWin32KeyboardDevice::Keys::kF10 ||
            (buttonEvent->GetIDCode() ==
                 RE::BSWin32KeyboardDevice::Keys::kEscape &&
             IsOpen()));
  }
  return false;
}
}  // namespace UI