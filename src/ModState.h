#pragma once

class ModState : public Singleton<ModState> {
 public:
  bool IsReady() const { return isReady_; }
  void SetReady(bool ready) { isReady_ = ready; }
  bool IsCSInstalled() const { return isCSInstalled_; }
  void SetCSInstalled(bool installed) { isCSInstalled_ = installed; }

 private:
  bool isReady_{false};
  bool isCSInstalled_{false};
};