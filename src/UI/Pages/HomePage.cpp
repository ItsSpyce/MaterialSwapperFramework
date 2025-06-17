#include "UI/Pages/HomePage.h"

#include "imgui.h"
#include "UI/Router.h"

void UI::Pages::HomePage(const HomePageProps& props) {
  ImGui::Text("Welcome to the Material Swapper Framework!");
  ImGui::Separator();
  ImGui::Text(
      "This is the home page where you can navigate to different sections.");
  ImGui::Separator();
  // Example links to other pages
  Link({"/materials", "View Materials"});
  Link({"/settings", "Settings"});
  // Add more content or navigation as needed
}