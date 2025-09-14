#pragma once

#include "Save/Save.h"

using namespace Save::V1;

class UniqueIDTable : public Singleton<UniqueIDTable>,
                      public ISaveable,
                      public RE::BSTEventSink<RE::TESContainerChangedEvent> {
  static constexpr u32 MAX_SAVE_LIFE = 100;  // seems reasonable enough?
  template <typename T, typename Container = std::deque<T> >
  class iterable_queue : public std::queue<T, Container> {
   public:
    typedef typename Container::const_iterator const_iterator;

    const_iterator begin() const { return this->c.begin(); }
    const_iterator end() const { return this->c.end(); }
  };

 public:
  UniqueIDTable();

  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESContainerChangedEvent* event,
      RE::BSTEventSource<RE::TESContainerChangedEvent>* src)
      override;
  void ReadFromSave(Save::SaveData& data) override;
  void WriteToSave(Save::SaveData& data) const override;
  u16 GetUID(RE::TESObjectREFR* refr, string& editorID, bool init);

  const string& GetEditorID(RE::FormID formID, u16 uid) {
    static constexpr string empty = "";
    if (uid == 0) {
      return empty;
    }
    auto full = formID & 0xFFFF0000 | uid;
    if (auto it = rows_.find(full); it != rows_.end()) {
      return it->second.editorID;
    }
    return empty;
  }

  RE::FormID GetOwnerID(RE::FormID formID, u16 uid) {
    if (uid == 0) {
      return 0;
    }
    auto full = formID & 0xFFFF0000 | uid;
    if (auto it = rows_.find(full); it != rows_.end()) {
      return it->second.ownerID;
    }
    return 0;
  }

 private:
  unordered_map<UniqueIDFull, UniqueIDRow> rows_;
  unordered_map<RE::FormID, UniqueID> nextUID_;
  unordered_map<RE::FormID, iterable_queue<UniqueID>> freeLists_;

  u16 GetNextAvailableUID(RE::FormID formID) {
    auto& freeList = freeLists_[formID];
    if (!freeList.empty()) {
      auto uid = freeList.front();
      freeList.pop();
      return uid;
    }
    auto& next = nextUID_[formID];
    if (next == 0) {
      next = 1;  // Start from 1, as 0 is reserved for "no UID"
    }
    return next++;
  }
};