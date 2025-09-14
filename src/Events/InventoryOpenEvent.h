#pragma once

namespace Events {
class InventoryOpenEvent : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
 public:
  RE::BSEventNotifyControl ProcessEvent(
      const RE::MenuOpenCloseEvent* a_event,
      RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
    if (a_event->menuName == RE::InventoryMenu::MENU_NAME && a_event->opening) {
      auto menu = RE::UI::GetSingleton()->GetMenu<RE::InventoryMenu>(
          RE::InventoryMenu::MENU_NAME);
      if (menu) {
        auto items = menu->itemList->items;
        for (auto& item : items) {
          // item->data.objDesc->extraLists->clear();
        }
      }
    }
    return RE::BSEventNotifyControl::kContinue;
  }
  static InventoryOpenEvent* Configure() {
    auto* events = new InventoryOpenEvent();
    auto* src = RE::UI::GetSingleton();
    src->AddEventSink(events);
    return events;
  }
};
}  // namespace Events