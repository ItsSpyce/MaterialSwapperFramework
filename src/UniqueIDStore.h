#pragma once

#include <lmdb.h>

namespace UniqueIDStore {
class UniqueIDDatabase {
public:

private:
  MDB_env* env;
};

inline UniqueID GetUniqueID(const RE::Actor* actor, RE::InventoryEntryData* invItem, bool init) {}
inline UniqueID GetUniqueID(const RE::Actor* actor, RE::BipedObjectSlot slot, bool init) {}
inline UniqueID GetUniqueID(const RE::Actor* actor, bool leftHand, bool init) {}
inline UniqueID GetUniqueID(const RE::TESObjectREFR* refr, bool init) {}

inline RE::FormID GetFormIDForUniqueID(const UniqueID uid) {}
}