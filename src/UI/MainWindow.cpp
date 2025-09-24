#include "Translations.h"
#include "UI.h"
#include "UI/Pages/DebuggerPage.h"
#include "UI/Pages/MaterialsPage.h"
#include "UI/Pages/SettingsPage.h"
#include "UI/Router.h"

namespace UI {

static void ConfigureStyles() {
  // Configure ImGui styles
  auto& style = ImGui::GetStyle();
  style.WindowRounding = 6.0f;
  style.FrameRounding = 4.0f;
  style.ChildRounding = 6.0f;
  style.PopupRounding = 4.0f;
  style.GrabRounding = 4.0f;
  style.ScrollbarRounding = 6.0f;
  style.WindowBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8, 6);
  style.FramePadding = ImVec2(10, 6);
  style.ItemInnerSpacing = ImVec2(6, 4);
  style.IndentSpacing = 20.0f;
  style.FrameBorderSize = 0.0f;
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);  // middle title
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ScrollbarSize = 12.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 4.0f;
}

void MainWindow::Initialize() {
  auto* router = Router::GetSingleton();
  router->UseRoute("/", [] { Pages::MaterialsPage({}); });
  router->UseRoute("/settings", [] { Pages::SettingsPage({}); });
  router->UseRoute("/debugger", [] {
    Pages::DebuggerPage({.actor = RE::PlayerCharacter::GetSingleton()});
  });
  SetWindowSize({1500, 600});
}

void MainWindow::Render() const {
  ConfigureStyles();
  ImGui_Window("MaterialSwapperFramework", NULL, ImGuiWindowFlags_NoTitleBar) {
    ImGui::Text(Translations::windowTitle());
    ImGui::Separator();
    ImGui::Text(Translations::showWindowInstructions());
    ImGui::Separator();
    ImGui_Child("Sidenav", ImVec2{150, 0}) {
      ImGui::Text(Translations::navigation());
      ImGui::Separator();
      Link({.path = "/", .label = Translations::homeTitle()});
      Link({.path = "/settings", .label = Translations::settingsTitle()});
      Link({.path = "/debugger", .label = Translations::debuggerTitle()});
    }
    ImGui::SameLine();
    ImGui_Child("Content") { Router::GetSingleton()->operator()(); }
  }
}
}  // namespace UI