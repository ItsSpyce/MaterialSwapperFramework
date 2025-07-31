#pragma once

#include "Factories.h"

namespace Events {
class NiNodeUpdateEvents : public RE::BSTEventSink<SKSE::NiNodeUpdateEvent> {
public:
  RE::BSEventNotifyControl ProcessEvent(
      const SKSE::NiNodeUpdateEvent* event,
      RE::BSTEventSource<SKSE::NiNodeUpdateEvent>* src) override {
    auto* actor = event->reference->As<RE::Actor>();
    if (!actor) {
      return RE::BSEventNotifyControl::kContinue;
    }
    logger::debug("NiNodeUpdateEvents: Actor: {}", actor->GetFormID());
    SKSE::GetTaskInterface()->AddTask([actor] {
      logger::info("Applying saved materials to equipped armor on load");
      auto* refr = RE::PlayerCharacter::GetSingleton();
      Factories::ArmorFactory::GetSingleton()->ApplySavedMaterials(actor);
    });
    return RE::BSEventNotifyControl::kContinue;
  }
  static NiNodeUpdateEvents* Configure() {
    auto* events = new NiNodeUpdateEvents();
    auto* src = SKSE::GetNiNodeUpdateEventSource();
    src->AddEventSink(events);
    return events;
  }
};
}