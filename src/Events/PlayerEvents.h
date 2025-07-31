#pragma once

#include "Factories.h"

namespace Events {
class PlayerEvents : public RE::BSTEventSink<RE::TESEquipEvent> {
  using ArmorFactory = Factories::ArmorFactory;

 public:
  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESEquipEvent* event,
      RE::BSTEventSource<RE::TESEquipEvent>* src) override {
    if (event->equipped) {
      auto refHandle = event->actor->GetHandle().get();
      SKSE::GetTaskInterface()->AddTask([refHandle] {
        ArmorFactory::GetSingleton()->ApplySavedMaterials(refHandle.get()->As<RE::Actor>());
      });
    }

    return RE::BSEventNotifyControl::kContinue;
  }

  static PlayerEvents* Configure() {
    const auto playerEvents = new PlayerEvents();
    if (const auto eventDispatcher =
            RE::ScriptEventSourceHolder::GetSingleton()) {
      eventDispatcher->AddEventSink<RE::TESEquipEvent>(playerEvents);
    }
    return playerEvents;
  }
};
}  // namespace Events