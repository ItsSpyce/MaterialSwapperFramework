#pragma once

#include "Singleton.h"

namespace UI {
struct RouterProps {
  // Define properties for the Router if needed
};

class Router : public Singleton<Router> {
 public:
  void operator()() {
    for (const auto& [route, render] : routes_) {
      if (currentPath_ == route) {
        render();
        return;
      }
    }
    ImGui::Text("404 Not Found: %s", currentPath_.c_str());
  }

  void UseRoute(const char* path, const std::function<void()>& callback) {
    // Register the route with the given path and callback
    routes_[path] = callback;
  }

  void NavigateTo(const char* path) {
    // Change the current path to the specified path
    currentPath_ = path;
  }

 private:
  std::unordered_map<std::string, std::function<void()>> routes_;
  std::string currentPath_ = "/";
};

struct LinkProps {
  const char* path;
  const char* label;
};

inline void Link(const LinkProps& props) {
  auto router = Router::GetSingleton();
  // Create a link that navigates to the specified path when clicked
  if (ImGui::Button(props.label)) {
    router->NavigateTo(props.path);
  }
}
}  // namespace UI