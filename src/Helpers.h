#pragma once

#include "EditorIDCache.h"
#include "NiOverride.h"
#include "UniqueIDTable.h"

namespace Helpers {

inline i32 GetModIndex(std::string_view name) {
  auto esp = RE::TESDataHandler::GetSingleton()->LookupModByName(name);
  if (!esp) return -1;
  return !esp->IsLight() ? esp->compileIndex << 24
                         : (0xFE000 | esp->smallFileCompileIndex) << 12;
}

inline u32 GetFormID(const std::string& name) {
  if (auto pos = name.find('|'); pos != std::string::npos) {
    auto i = GetModIndex(name.substr(0, pos));
    return i | std::stoul(name.substr(pos + 1), nullptr, 16);
  }
  if (name.starts_with("0x")) {
    return std::stoul(name.substr(2), nullptr, 16);
  }
  if (name.starts_with("0X")) {
    return std::stoul(name.substr(2), nullptr, 16);
  }
  if (const auto formIDFromCache = EditorIDCache::GetFormID(name);
      formIDFromCache != NULL) {
    return formIDFromCache;
  }
  return 0;  // Return 0 if not found
}

inline UniqueID GetUniqueID(RE::TESObjectREFR* refr,
                            const RE::InventoryEntryData* data, bool init) {
  if (!refr || !data) {
    return 0;
  }
  auto* armo = data->object->As<RE::TESObjectARMO>();
  if (!armo) {
    return 0;
  }
  return NiOverride::GetItemUniqueID()(
      RE::StaticFunctionTag{}, refr, 0,
      static_cast<int>(armo->GetSlotMask()), init);
  /*auto* armo = data->object->As<RE::TESObjectARMO>();
  if (!armo) {
    return 0;
  }
  return UniqueIDTable::GetSingleton()->GetUID(refr, armo->GetFormID(), init);*/
}

inline UniqueID GetUniqueID(RE::TESObjectREFR* refr,
                            RE::BGSBipedObjectForm::BipedObjectSlot slot,
                            bool init) {
  if (!refr && !init) {
    return 0;
  }
  return NiOverride::GetItemUniqueID()(RE::StaticFunctionTag{}, refr, 0,
                                       static_cast<int>(slot), init);
  /*auto* actor = refr->As<RE::Actor>();
  if (!actor) {
    return 0;
  }
  if (auto* armo = actor->GetWornArmor(slot)) {
    return UniqueIDTable::GetSingleton()->GetUID(refr, armo->GetFormID(), init);
  }
  _WARN("No armor found for slot: {}", static_cast<int>(slot));
  return 0;*/
}

inline RE::FormID GetFormIDForUniqueID(UniqueID uid) {
  if (uid == NULL) {
    return 0;
  }
  auto* form = NiOverride::GetFormFromUniqueID()(RE::StaticFunctionTag{}, uid);
  return form ? form->GetFormID() : 0;
  // return UniqueIDTable::GetSingleton()->GetFormID(uid);
}

struct InventoryItem {
  RE::TESBoundObject* object;
  i32 count;
  std::unique_ptr<RE::InventoryEntryData> data;
  UniqueID uid;
};

inline void VisitInventoryItems(RE::TESObjectREFR* refr,
                                const Visitor<InventoryItem*>& visitor) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto uid = GetUniqueID(refr, data.second.get(), false);
    auto* inventoryItem = new InventoryItem{.object = obj,
                                            .count = data.first,
                                            .data = std::move(data.second),
                                            .uid = uid};
    visitor(inventoryItem);
  }
}

inline void VisitEquippedInventoryItems(
    RE::TESObjectREFR* refr, const Visitor<InventoryItem*>& visitor) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second || !data.second->extraLists) {
      continue;  // Skip if object or data is null
    }
    auto uid = GetUniqueID(refr, data.second.get(), false);
    for (auto* extraList : *data.second->extraLists) {
      if (!extraList) {
        continue;
      }
      if (extraList->HasType(RE::ExtraDataType::kWorn)) {
        auto* inventoryItem = new InventoryItem{.object = obj,
                                                .count = data.first,
                                                .data = std::move(data.second),
                                                .uid = uid};
        visitor(inventoryItem);
      }
    }
  }
}

inline InventoryItem* GetInventoryItemWithFormID(RE::TESObjectREFR* refr,
                                                 RE::FormID formID) {
  auto inventoryData = refr->GetInventory(
      [&](const RE::TESBoundObject& obj) { return obj.GetFormID() == formID; });
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto uid = GetUniqueID(refr, data.second.get(), false);

    return new InventoryItem{.object = obj,
                             .count = data.first,
                             .data = std::move(data.second),
                             .uid = uid};
  }
  return nullptr;
}

inline InventoryItem* GetInventoryItemWithUID(RE::TESObjectREFR* refr,
                                              UniqueID uid) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto itemUID = GetUniqueID(refr, data.second.get(), false);
    if (itemUID == uid) {
      return new InventoryItem{.object = obj,
                               .count = data.first,
                               .data = std::move(data.second),
                               .uid = itemUID};
    }
  }
  return nullptr;
}

template <typename T>
T* GetOrCreateType(RE::ExtraDataList* list, function<T*()> configure) {
  if (!list) {
    return nullptr;
  }
  if (auto* extra = list->GetByType<T>()) {
    return extra;
  }
  auto* extra = configure();
  list->Add(extra);
  return extra;
}

inline RE::ExtraDataList* ConstructExtraDataList(void* a_this) {
  using func_t = decltype(&ConstructExtraDataList);
  REL::Relocation<func_t> func{RELOCATION_ID(11437, 11583)};
  return func(a_this);
}

inline RE::ExtraDataList* GetOrCreateExtraList(RE::InventoryEntryData* data) {
  if (!data) {
    return nullptr;
  }
  if (!data->extraLists) {
    data->extraLists = new RE::BSSimpleList<RE::ExtraDataList*>();
  }
  if (!data->extraLists->empty()) {
    return data->extraLists->front();
  }
  const auto memoryManager = RE::MemoryManager::GetSingleton();
  auto alloc = memoryManager->Allocate(sizeof(RE::ExtraDataList), 0, false);
  auto* newList = ConstructExtraDataList(alloc);

  data->AddExtraList(newList);
  return newList;
}
}  // namespace Helpers