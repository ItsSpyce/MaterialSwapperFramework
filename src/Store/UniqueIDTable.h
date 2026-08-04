#pragma once

#include <emhash/hash_table5.hpp>

#include "Types.h"
#include "Helpers/SkyrimHelpers.h"

class UniqueIDTable {
 public:
 private:
  emhash5::HashMap<UniqueID, RE::FormID> refUids_;
  emhash5::HashMap<UniqueID, i32> rankUids_;

  UniqueID GetUniqueIDForInventoryItem(RE::InventoryEntryData* data,
                                       bool init = false) {
    if (!data) return 0;
    for (const auto& extraList : *data->extraLists) {
      if (extraList) {
        if (data->object->IsArmor()) {
          if (auto* extraHealth = extraList->GetByType<RE::ExtraHealth>()) {
            return static_cast<UniqueID>(extraHealth->health);
          }
        } else if (data->object->IsWeapon()) {
          //
        }
      }
    }
    if (init) {
      auto* extraList = Helpers::GetOrCreateExtraList(data);
      if (data->object->IsArmor()) {
        
      }
    }
    return 0;
  }
};