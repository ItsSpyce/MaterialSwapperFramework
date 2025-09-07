#pragma once

class ModState : public Singleton<ModState> {
 public:
  bool IsReady() const { return isReady_; }
  void SetReady(bool ready) { isReady_ = ready; }

 private:
  bool isReady_{false};
};