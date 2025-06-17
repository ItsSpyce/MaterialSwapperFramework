#pragma once

namespace UI {
using WindowManager = ImGui::SKSE::WindowManager<[] { return true; }, false>;

class MainWindow final : public WindowManager::UIWindow {
public:
  void Initialize() override;
  void Update(uint32_t timer) override {}
  void Draw() override;

  bool IsEnabled() const override { return true; }

  bool IsShowKey(RE::InputEvent* event) override;

  constexpr uint8_t GetWindowType() override {
    return WindowManager::WindowType::WindowType_kMenu;
  }
};

static void Initialize() {
  WindowManager::RegisterWindow<MainWindow>();
  WindowManager::Initialize();
}

static void RegisterHooks() { WindowManager::RegisterHooks(); }
}  // namespace UI