#pragma once

#include "EditorIDCache.h"
#include "Save/Save.h"

using namespace Save::V1;
using namespace Save::Types;

// from the save data, UniqueID is stored as u64 (RE::FormID:u32 | uid:u16 |
// padding:u16)

struct UniqueIDChangedEvent {
  UniqueID oldUID;
  UniqueID newUID;
};

class UniqueIDTable final
    : public Singleton<UniqueIDTable>,
      public ISaveable,
      public RE::BSTEventSink<RE::TESContainerChangedEvent>,
      public RE::BSTEventSource<UniqueIDChangedEvent> {
  static constexpr u32 MAX_SAVE_LIFE = 100;  // seems reasonable enough?
  template <typename T, typename Container = std::deque<T>>
  class iterable_queue : public std::queue<T, Container> {
   public:
    typedef typename Container::const_iterator const_iterator;

    const_iterator begin() const { return this->c.begin(); }
    const_iterator end() const { return this->c.end(); }
  };
  using Lock = std::mutex;
  using Locker = std::scoped_lock<Lock>;

 public:
  UniqueIDTable();

  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESContainerChangedEvent* event,
      BSTEventSource<RE::TESContainerChangedEvent>* src) override;
  void ReadFromSave(SKSE::SerializationInterface* iface, Save::SaveData& data) override;
  void WriteToSave(SKSE::SerializationInterface* iface, Save::SaveData& data) override;
  UniqueID GetUID(RE::TESObjectREFR* refr, RE::FormID formID, bool init);

  RE::FormID GetFormID(const UniqueID uniqueID) const {
    if (uniqueID == 0) {
      return 0;
    }
    RE::FormID formID;
    u16 uid;
    DecodeUID(uniqueID, formID, uid);
    return formID;
  }

  RE::VMHandle GetOwnerID(const UniqueID uniqueID) {
    Locker lock(lock_);
    if (uniqueID == 0) {
      return 0;
    }
    if (const auto it = rows_.find(uniqueID); it != rows_.end()) {
      return it->second.ownerID;
    }
    return 0;
  }

  RE::VMHandle GetOwnerID(const RE::FormID formID, u16 uid) {
    if (uid == 0) {
      return 0;
    }
    const auto fullID = EncodeUID(formID, uid);
    return GetOwnerID(fullID);
  }

  void ClearAllData() {
    Locker lock(lock_);
    rows_.clear();
    nextUID_.clear();
    freeLists_.clear();
  }

 private:
  struct UniqueIDRow {
    RE::VMHandle ownerID;
    mutable u32 savesSinceLastAccess;
    UniqueItemLocation location;
  };

  mutable Lock lock_;
  unordered_map<UniqueID, UniqueIDRow> rows_;
  unordered_map<RE::FormID, u16> nextUID_;
  unordered_map<RE::FormID, iterable_queue<u16>> freeLists_;

  static UniqueID EncodeUID(RE::FormID formID, u16 uid) {
    // a full ID consists of the formID (u32) in the high bits and the uid (u16)
    // in the low bits
    return (static_cast<u64>(formID) << 16) | static_cast<u64>(uid);
  }

  static void DecodeUID(u64 fullID, RE::FormID& formID, u16& uid) {
    formID = static_cast<RE::FormID>(fullID >> 16);
    uid = static_cast<u16>(fullID & 0xFFFF);
  }

  u16 GetNextAvailableUID(RE::FormID formID) {
    Locker lock(lock_);
    if (formID == 0) {
      return 0;
    }
    if (!nextUID_.contains(formID)) {
      nextUID_.emplace(formID, 1);
    }
    auto& nextUID = nextUID_[formID];
    auto& freeList = freeLists_[formID];
    if (!freeList.empty()) {
      auto uid = freeList.front();
      freeList.pop();
      return uid;
    }
    return nextUID++;
  }
};