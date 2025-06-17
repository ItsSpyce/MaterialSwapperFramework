#pragma once

class Events : public RE::BSTEventSink<RE::TESEquipEvent>,
               public RE::BSTEventSink<RE::TESLoadGameEvent>,
               public RE::BSTEventSink<SKSE::ModCallbackEvent> {
 public:
  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESEquipEvent* event,
      RE::BSTEventSource<RE::TESEquipEvent>* src) override;

  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESLoadGameEvent* event,
      RE::BSTEventSource<RE::TESLoadGameEvent>* src) override;

  RE::BSEventNotifyControl ProcessEvent(
      const SKSE::ModCallbackEvent* event,
      RE::BSTEventSource<SKSE::ModCallbackEvent>* src) override;

  static Events* Configure() {
    const auto kitchenSink = new Events();
    if (const auto eventDispatcher =
            RE::ScriptEventSourceHolder::GetSingleton()) {
      eventDispatcher->AddEventSink<RE::TESEquipEvent>(kitchenSink);
      eventDispatcher->AddEventSink<RE::TESLoadGameEvent>(kitchenSink);
    }
    if (const auto modCallbackDispatcher = SKSE::GetModCallbackEventSource()) {
      modCallbackDispatcher->AddEventSink<SKSE::ModCallbackEvent>(kitchenSink);
    }
    return kitchenSink;
  }
};
