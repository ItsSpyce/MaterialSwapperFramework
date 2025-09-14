#pragma once

namespace Events {
class PapyrusEvent : public RE::BSTEventSink<SKSE::ModCallbackEvent> {
 public:
  RE::BSEventNotifyControl ProcessEvent(
      const SKSE::ModCallbackEvent* event,
      RE::BSTEventSource<SKSE::ModCallbackEvent>* src) override {
    return RE::BSEventNotifyControl::kContinue;
  }

  static PapyrusEvent* Configure() {
    const auto playerEvents = new PapyrusEvent();
    if (const auto modCallbackDispatcher = SKSE::GetModCallbackEventSource()) {
      modCallbackDispatcher->AddEventSink<SKSE::ModCallbackEvent>(playerEvents);
    }
    return playerEvents;
  }
};
}  // namespace Events