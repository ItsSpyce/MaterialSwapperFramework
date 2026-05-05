#pragma once

namespace Events {
class MSFEvent {
public:
  void Cancel() { canceled_ = true; }
  NODISCARD bool IsCanceled() const { return canceled_; }

private:
  bool canceled_{false};
};
}