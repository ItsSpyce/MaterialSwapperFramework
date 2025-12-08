#pragma once

#include "UIManager.h"
#ifdef UI_USE_IMGUI
#include "ImGui/UIManagerImpl.h"
#endif
#ifdef UI_USE_LCUI
#include "LCUI/UIManagerImpl.h"
#endif
#ifdef UI_USE_NUKLEAR
#include "Nuklear/UIManagerImpl.h"
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

inline void Install() { Hooks::Install(); }

inline void Initialize() {
  if (auto* ui = GetCurrentUI()) {
    ui->AddWindow(MainWindow::GetSingleton());
    ui->Initialize();
  }
}
}  // namespace UI