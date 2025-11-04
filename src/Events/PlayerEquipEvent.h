#pragma once

#include "Factories.h"

namespace Events {
class PlayerEquipEvent : public RE::BSTEventSink<RE::TESEquipEvent> {
  using ArmorFactory = Factories::ArmorFactory;

 public:
  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESEquipEvent* event,
      RE::BSTEventSource<RE::TESEquipEvent>* src) override {
    if (event->equipped) {
      auto* form =
          RE::TESForm::LookupByID<RE::TESObjectARMO>(event->baseObject);
      if (!form) {
        // not an armor item or not found
        return RE::BSEventNotifyControl::kContinue;
      }
      auto refHandle = event->actor->GetHandle().get();
      /*SKSE::GetTaskInterface()->AddTask([refHandle, form] {
        ArmorFactory::GetSingleton()->ApplySavedMaterials(
            refHandle->As<RE::Actor>(), form);
      });*/
    }

    return RE::BSEventNotifyControl::kContinue;
  }

  static PlayerEquipEvent* Configure() {
    const auto playerEvents = new PlayerEquipEvent();
    if (const auto eventDispatcher =
            RE::ScriptEventSourceHolder::GetSingleton()) {
      eventDispatcher->AddEventSink<RE::TESEquipEvent>(playerEvents);
    }
    return playerEvents;
  }
};
}  // namespace Events