#pragma once

#include "UIManager.h"

namespace UI {
class MainWindow final : public Window, public Singleton<MainWindow> {
 public:
  void Initialize() override;
  void Update(uint32_t timer) override {}
  void Render() const override;
};
}  // namespace UI