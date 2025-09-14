#include "UniqueIDTable.h"

#include "EditorIDCache.h"
#include "Save/Save.h"

void UniqueIDTable::ReadFromSave(Save::SaveData& data) {
  rows_.clear();
  for (const auto& row : data.uniqueIDHistory.rows) {
    row.savesSinceLastAccess++;
    if (row.savesSinceLastAccess > MAX_SAVE_LIFE) {
      _TRACE("Removing stale UID: {} for owner: {}, form: {}", row.uid,
             row.ownerID, row.editorID);
      freeList_.emplace(row.uid);
      continue;
    }
    rows_[row.uid] = row;
  }
  nextUID_ = data.uniqueIDHistory.nextUID;
}

void UniqueIDTable::WriteToSave(Save::SaveData& data) const {
  auto values = rows_ | views::values;
  data.uniqueIDHistory.rows = vector(values.begin(), values.end());
  data.uniqueIDHistory.nextUID = nextUID_;
  data.uniqueIDHistory.freedUIDs = vector(freeList_.begin(), freeList_.end());
}

u16 UniqueIDTable::GetUID(RE::TESObjectREFR* refr, string& editorID,
                          bool init) {
  if (!refr) {
    return 0;
  }
  if (editorID.empty()) {
    _ERROR("Empty editor ID provided for reference: {}", refr->GetFormID());
    return 0;
  }
  const auto formID = EditorIDCache::GetFormID(editorID);

  auto inventory = refr->GetInventory();
  for (auto& [obj, data] : inventory) {
    if (obj->GetFormID() != formID) {
      continue;
    }
    if (!obj || !data.second || !data.second->extraLists) {
      continue;  // Skip if object or data is null
    }
    auto* lists = data.second->extraLists;
    if (lists->empty()) {
      continue;
    }
    auto* front = lists->front();
    if (auto* uidExtra = front->GetByType<RE::ExtraUniqueID>()) {
      return uidExtra->uniqueID;
    } else if (init) {
      uidExtra = new RE::ExtraUniqueID(formID, GetNextAvailableUID());
      front->Add(uidExtra);
      front->Add(new RE::ExtraRank(0xFFFFFFFF));
      rows_.emplace(uidExtra->uniqueID,
                    UniqueIDRow{.uid = uidExtra->uniqueID,
                                .ownerID = refr->GetFormID(),
                                .editorID = string(editorID),
                                .savesSinceLastAccess = 0,
                                .index = 0,
                                .location = UniqueItemLocation::kInventory});
      return uidExtra->uniqueID;
    } else {
      return 0;
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
    auto refID = event->reference.native_handle();
    auto row = rows_.at(event->uniqueID);
    row.ownerID = refID;
    row.location = UniqueItemLocation::kWorld;
    auto* reference = RE::TESObjectARMO::LookupByID(refID);
    _TRACE("Dropped item with UID: {} from owner: {} into the world",
           event->uniqueID, reference ? reference->GetName() : "unknown");
  } else if (!oldContainer) {
    // item was picked up from the world
  }
  return RE::BSEventNotifyControl::kContinue;
}

UniqueIDTable::UniqueIDTable() {
  RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(this);
}