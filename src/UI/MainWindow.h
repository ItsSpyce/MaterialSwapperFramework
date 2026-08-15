#pragma once

#include "UIManager.h"

namespace UI {
class MainWindow final : public Window, public Singleton<MainWindow> {
 public:
  void Initialize() override;
  void Update(float) override {}
  void Render() const override;
  void Show() override;
  void Hide() override;

private:
  RE::NiPointer<RE::NiAVObject> preview_;
};
}  // namespace UI