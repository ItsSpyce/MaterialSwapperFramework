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
      auto refHandle = event->actor->GetHandle().get();
      auto baseObject = event->baseObject + 0;  // Ensure baseObject is not null
      SKSE::GetTaskInterface()->AddTask([refHandle, baseObject] {
        auto inventoryItem =
            Helpers::GetInventoryItemWithFormID(refHandle.get(), baseObject);
        ArmoFactory::GetSingleton()->ApplySavedMaterial(refHandle.get(),
                                                        inventoryItem);
      });
    }

    return RE::BSEventNotifyControl::kContinue;
  }

  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESLoadGameEvent* event,
      RE::BSTEventSource<RE::TESLoadGameEvent>* src) override {
    SKSE::GetTaskInterface()->AddTask([&] {
      logger::info("Applying saved materials to equipped armor on load");
      Helpers::VisitEquippedInventoryItems(
          RE::PlayerCharacter::GetSingleton(),
          [](const std::unique_ptr<Helpers::InventoryItem>& item) {
            if (item->data->object->IsArmor()) {
              ArmoFactory::GetSingleton()->ApplySavedMaterial(
                  RE::PlayerCharacter::GetSingleton(), item);
            }
          });
    });
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