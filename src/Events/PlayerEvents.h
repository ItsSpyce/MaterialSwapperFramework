#pragma once

#include "Factories.h"

namespace Events {
class PlayerEvents : public RE::BSTEventSink<RE::TESEquipEvent>,
                     public RE::BSTEventSink<RE::TESLoadGameEvent> {
  using ArmoFactory = Factories::ArmorFactory;

 public:
  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESEquipEvent* event,
      RE::BSTEventSource<RE::TESEquipEvent>* src) override {
    if (event->equipped) {
      RE::NiPointer actorRef(event->actor);
      SKSE::GetTaskInterface()->AddTask([actorRef] {
        ArmoFactory::GetSingleton()->ApplySavedMaterial(actorRef.get(), NULL);
      });
    }

    return RE::BSEventNotifyControl::kContinue;
  }

  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESLoadGameEvent* event,
      RE::BSTEventSource<RE::TESLoadGameEvent>* src) override {
    ArmoFactory::GetSingleton()->ApplySavedMaterial(
        RE::PlayerCharacter::GetSingleton(), NULL);
    return RE::BSEventNotifyControl::kContinue;
  }

  static PlayerEvents* Configure() {
    const auto playerEvents = new PlayerEvents();
    if (const auto eventDispatcher =
            RE::ScriptEventSourceHolder::GetSingleton()) {
      eventDispatcher->AddEventSink<RE::TESEquipEvent>(playerEvents);
      eventDispatcher->AddEventSink<RE::TESLoadGameEvent>(playerEvents);
    }
    return playerEvents;
  }
};
}  // namespace Events