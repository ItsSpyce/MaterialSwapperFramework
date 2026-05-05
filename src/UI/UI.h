#pragma once

#include "UIManager.h"
#ifdef UI_USE_IMGUI
#include "ImGui/UIManagerImpl.h"
#elif UI_USE_PRISMA
#include "Prisma/UIManagerImpl.h"
#endif
#include "UI/Hooks.h"
#include "MainWindow.h"

namespace UI {
inline UIManager* GetCurrentUI() {
  static UIManager* ui;
  if (!ui) {
    ui = new UIManagerImpl();
  }
  return ui;
}

inline void Install() {
#ifdef UI_USE_IMGUI
  Hooks::Install();
#endif
}

inline void Initialize() {
  if (auto* ui = GetCurrentUI()) {
    ui->AddWindow(MainWindow::GetSingleton());
    ui->Initialize();
  }
}
}  // namespace UI