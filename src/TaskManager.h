#pragma once

#include "Events/EventListener.h"
#include <functional>
#include <queue>
#include <atomic>

struct FrameEvent {
  bool gamePaused;
};

struct ArmorAttachEvent {
  RE::Actor* actor;
  RE::NiNode* armor;
  RE::NiNode* skeleton;
  RE::NiAVObject* attachedAt;
  u32 bipedSlot;
  bool hasAttached = false;
};

struct PlayerCellChangeEvent {
  bool isExterior = false;
  bool isChangedInOut = false;
};

class TaskManager : public Singleton<TaskManager>,
                    public EventListener<FrameEvent>,
                    public EventListener<ArmorAttachEvent>,
                    public EventListener<PlayerCellChangeEvent>,
                    public RE::BSTEventSink<SKSE::NiNodeUpdateEvent> {
 public:
  TaskManager() = default;
  ~TaskManager() override = default;

  void Initialize() {
    SKSE::GetNiNodeUpdateEventSource()->AddEventSink(this);
  }

  void OnEvent(const ArmorAttachEvent& event) override {
    if (!event.actor || !event.hasAttached) {
      return;
    }
    RE::FormID formID = event.actor->GetFormID();
    RegisterDelayTask([this, formID] {
      if (auto* actor = RE::TESForm::LookupByID<RE::Actor>(formID)) {
        Factories::ArmorFactory::GetSingleton()->ApplySavedMaterials(actor);
      }
    }, 1);
  }

  void OnEvent(const FrameEvent& event) override {
    if (!canRunTasks_.load()) {
      return;
    }
    size_t taskCount = delayedTasks_.size();
    for (size_t i = 0; i < taskCount; i++) {
      auto& task = delayedTasks_.front();
      if (task.remainingTicks == 0) {
        task();
        delayedTasks_.pop();
      } else {
        task.remainingTicks--;
        delayedTasks_.push(task);
        delayedTasks_.pop();
      }
    }
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