#include "UniqueIDTable.h"

#include <unordered_set>

#include "Helpers.h"
#include "Save/Save.h"

inline bool TryResolveFormID(const SKSE::SerializationInterface* iface,
                             RE::FormID& formID) {
  RE::FormID newFormID;
  if (!iface->ResolveFormID(formID, newFormID)) {
    return false;
  }
  formID = newFormID;
  return true;
}

void UniqueIDTable::ReadFromSave(SKSE::SerializationInterface* iface,
                                 Save::SaveData& data) {
  rows_.clear();
  freeLists_.clear();
  nextUID_.clear();
  unordered_map<RE::FormID, u16> nextUID;
  for (const auto& row : data.uniqueIDHistory.rows) {
    RE::FormID formID, newFormID;
    u16 uid;
    DecodeUID(row.uniqueID, formID, uid);
    if (!iface->ResolveFormID(formID, newFormID)) {
      _WARN("Failed to resolve formID for uniqueID: {}", row.uniqueID);
      continue;
    }
    if (formID != newFormID) {
      _TRACE("Moved form ID {} to {}", formID, newFormID);
      formID = newFormID;
    }
    RE::VMHandle ownerID;
    if (!iface->ResolveHandle(row.ownerID, ownerID)) {
      _WARN("Failed to resolve ownerID for uniqueID: {}", row.uniqueID);
      continue;
    }
    rows_.emplace(EncodeUID(formID, uid),
                  UniqueIDRow{.ownerID = ownerID,
                              .savesSinceLastAccess = row.savesSinceLastAccess,
                              .location = row.location});
    if (nextUID.contains(formID)) {
      if (uid >= nextUID[formID]) {
        nextUID[formID] = uid + 1;
      }
    } else {
      nextUID[formID] = uid + 1;
    }
  }
  nextUID_ = std::move(nextUID);
  for (const auto& uniqueID : data.uniqueIDHistory.freedUIDs) {
    RE::FormID formID;
    u16 uid;
    DecodeUID(uniqueID, formID, uid);
    if (freeLists_.contains(formID)) {
      freeLists_[formID].push(uniqueID);
    } else {
      iterable_queue<u16> queue;
      queue.push(uniqueID);
      freeLists_.emplace(formID, std::move(queue));
    }
  }
}

void UniqueIDTable::WriteToSave(SKSE::SerializationInterface* iface,
                                Save::SaveData& data) {
  Locker lock(lock_);
  data.uniqueIDHistory.rows.clear();
  data.uniqueIDHistory.freedUIDs.clear();
  for (const auto& [uniqueID, row] : rows_) {
    data.uniqueIDHistory.rows.push_back(UniqueIDRowV2{
        .uniqueID = uniqueID,
        .ownerID = row.ownerID,
        .savesSinceLastAccess = row.savesSinceLastAccess,
        .location = row.location,
    });
  }
  for (const auto& [formID, queue] : freeLists_) {
    for (const auto& uid : queue) {
      const auto fullID = EncodeUID(formID, uid);
      data.uniqueIDHistory.freedUIDs.push_back(fullID);
    }
  }
}

UniqueID UniqueIDTable::GetUID(RE::TESObjectREFR* refr, const RE::FormID formID,
                               bool init) {
  // TODO: rework this whole function. Main things it needs:
  // - we should probably pass in the whole RE::TESBoundObject because we need
  //   to test if there's a UID already for that single item
  // - let the UI differentiate between multiple items in a stack. To do this,
  //   lets get renaming working as a foot-hold
  if (!refr) {
    return 0;
  }
  Locker lock(lock_);

  auto inventory = refr->GetInventory(
      [&](const RE::TESBoundObject& obj) { return obj.GetFormID() == formID; });
  for (auto& data : inventory | views::values) {
    auto* front = Helpers::GetOrCreateExtraList(data.second.get());
    if (!front) {
      _ERROR("Failed to get or create ExtraDataList for item in inventory");
      return 0;
    }

    auto* uidExtra = front->GetByType<RE::ExtraUniqueID>();
    if (uidExtra && uidExtra->uniqueID != 0) {
      if (init) {
        nextUID_.try_emplace(formID, 1);
        auto& nextUID = nextUID_.at(formID);
        if (uidExtra->uniqueID >= nextUID) {
          nextUID += uidExtra->uniqueID + 1;
        }
      }
      const auto fullID = EncodeUID(formID, uidExtra->uniqueID);
      if (auto it = rows_.find(fullID); it == rows_.end()) {
        if (init) {
          rows_.emplace(
              fullID, UniqueIDRow{.ownerID = refr->GetFormID(),
                                  .savesSinceLastAccess = 0,
                                  .location = UniqueItemLocation::kInventory});
        }
      } else {
        it->second.savesSinceLastAccess = 0;
        it->second.location = UniqueItemLocation::kInventory;
      }
      return fullID;
    }
    if (init) {
      _TRACE("Item has no UID, assigning new one");
      uidExtra = new RE::ExtraUniqueID(formID, GetNextAvailableUID(formID));
      if (uidExtra->uniqueID == 0) {
        _ERROR("UID is zero for item in inventory!");
        return 0;
      }
      auto* rankExtra = new RE::ExtraRank(0xFFFFFFFF);
      front->Add(uidExtra);
      front->Add(rankExtra);
      const auto fullID = EncodeUID(formID, uidExtra->uniqueID);
      rows_.emplace(fullID,
                    UniqueIDRow{.ownerID = refr->GetFormID(),
                                .savesSinceLastAccess = 0,
                                .location = UniqueItemLocation::kInventory});
      _TRACE("Assigned new UID for item in inventory: {}", fullID);
      return fullID;
    }
  }
  return 0;
}

RE::BSEventNotifyControl UniqueIDTable::ProcessEvent(
    const RE::TESContainerChangedEvent* event,
    RE::BSTEventSource<RE::TESContainerChangedEvent>*) {
  if (event->uniqueID == 0) {
    // no UID, ignore
    return RE::BSEventNotifyControl::kContinue;
  }
  if (event->itemCount != 1) {
    // items with UIDs can not be stacked, so ignore events that move more
    return RE::BSEventNotifyControl::kContinue;
  }
  auto* oldContainer = RE::TESForm::LookupByID(event->oldContainer);
  auto* newContainer = RE::TESForm::LookupByID(event->newContainer);
  if (!newContainer && event->reference) {
    // item was dropped into the world

  } else if (!oldContainer) {
    // item was picked up from the world
  }
  return RE::BSEventNotifyControl::kContinue;
}

UniqueIDTable::UniqueIDTable() {
  RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(this);
}