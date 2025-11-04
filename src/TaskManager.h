#pragma once

#include <atomic>
#include <functional>
#include <queue>

#include "Events/EventListener.h"

extern EventSource<FrameEvent> g_frameEventSource;
extern EventSource<ArmorAttachEvent> g_armorAttachSource;
extern EventSource<PlayerCellChangeEvent> g_cellChangeSource;

class TaskManager final : public Singleton<TaskManager>,
                          public EventListener<FrameEvent>,
                          public EventListener<ArmorAttachEvent>,
                          public EventListener<PlayerCellChangeEvent> {
 public:
  TaskManager() = default;
  ~TaskManager() = default;

  void Initialize() {
    // todo?
  }

  void OnEvent(const ArmorAttachEvent& event) override {
    if (!event.actor || !event.hasAttached) {
      return;
    }
    auto formID = event.actor->GetFormID();
    RegisterDelayTask(
        [this, formID, event] {
          if (auto* actor = RE::TESForm::LookupByID<RE::Actor>(formID)) {
            Factories::ArmorFactory::GetSingleton()->ApplySavedMaterials(
                actor, event.armor, event.attachedAt, event.bipedSlot);
          }
        },
        1);
  }

  void OnEvent(const FrameEvent& event) override {
    if (!canRunTasks_.load()) {
      return;
    }
    const auto taskCount = delayedTasks_.size();
    for (size_t i = 0; i < taskCount; i++) {
      if (auto& task = delayedTasks_.front(); task.remainingTicks == 0) {
        task();
        delayedTasks_.pop();
      } else {
        task.remainingTicks--;
        delayedTasks_.push(task);
        delayedTasks_.pop();
      }
    }
  }

  void OnEvent(const PlayerCellChangeEvent& event) override {
    canRunTasks_.store(true);
  }

  void RegisterDelayTask(const function<void()>& func, u16 delayTick) {
    delayedTasks_.push(DelayedTask{func, delayTick});
  }

 private:
  struct DelayedTask {
    std::function<void()> func;
    u16 remainingTicks;

    // Keep a non-const call operator for convenience.
    void operator()() const {
      if (func) {
        func();
      }
    }
  };
  std::queue<DelayedTask> delayedTasks_;
  std::atomic<bool> canRunTasks_{false};
};