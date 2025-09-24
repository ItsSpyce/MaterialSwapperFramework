#pragma once

#include "Save/Save.h"

class InventoryPlus : public Singleton<InventoryPlus>,
                      public ISaveable,
                      public RE::BSTEventSink<RE::TESContainerChangedEvent> {
  using Lock = std::mutex;
  using Locker = std::scoped_lock<Lock>;

  struct InventoryRecord {
    
  };

 public:
  void ReadFromSave(SKSE::SerializationInterface* iface,
                    Save::SaveData& saveData) override;
  void WriteToSave(SKSE::SerializationInterface* iface,
                   Save::SaveData& saveData) override;
  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESContainerChangedEvent* event,
      RE::BSTEventSource<RE::TESContainerChangedEvent>* src) override;

 private:
  
};