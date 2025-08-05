#pragma once

#include "Factories.h"

namespace Events {
class CellEvents : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent> {
 public:
  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESCellFullyLoadedEvent* a_event,
      RE::BSTEventSource<RE::TESCellFullyLoadedEvent>*) override {
    a_event->cell->ForEachReference([&](RE::TESObjectREFR* refr) {
      auto* actor = refr->As<RE::Actor>();
      if (!actor) {
        return RE::BSContainer::ForEachResult::kContinue;
      }
      logger::debug("CellEvents: Actor: {}", actor->GetFormID());
      Factories::ArmorFactory::GetSingleton()->ApplySavedMaterials(actor);
      return RE::BSContainer::ForEachResult::kContinue;
    });
    return RE::BSEventNotifyControl::kContinue;
  }

  static CellEvents* Configure() {
    auto* events = new CellEvents();
    auto* src = RE::ScriptEventSourceHolder::GetSingleton();
    src->AddEventSink(events);
    return events;
  }
};
}  // namespace Events