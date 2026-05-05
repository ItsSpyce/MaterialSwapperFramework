#pragma once
#include "EditorIDCache.h"
#include "RaceMenuHelpers.h"
#include "StringHelpers.h"

namespace Helpers {
inline i32 GetModIndex(const std::string_view name) {
  auto* esp = RE::TESDataHandler::GetSingleton()->LookupModByName(name);
  if (!esp) return -1;
  return !esp->IsLight() ? esp->compileIndex << 24
                         : (0xFE000 | esp->smallFileCompileIndex) << 12;
}

inline u32 GetFormID(const std::string& name) {
  if (StringHelpers::ToLower(name) == "player") {
    return RE::PlayerCharacter::GetSingleton()->GetFormID();
  }
  if (const auto pos = name.find('|'); pos != std::string::npos) {
    const auto i = GetModIndex(name.substr(0, pos));
    return i | std::stoul(name.substr(pos + 1), nullptr, 16);
  }
  if (name[0] == '0' && (name[1] == 'x' || name[1] == 'X')) {
    return std::stoul(name.substr(2), nullptr, 16);
  }
  if (const auto formIDFromCache = EditorIDCache::GetFormID(name);
      formIDFromCache != NULL) {
    return formIDFromCache;
  }
  return NULL;  // return NULL if not found
}

inline void VisitInventoryItems(RE::TESObjectREFR* refr,
                                const Visitor<InventoryItem*>& visitor) {
  for (auto inventoryData = refr->GetInventory();
       auto& [obj, data] : inventoryData) {
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
  for (auto inventoryData = refr->GetInventory();
       auto& [obj, data] : inventoryData) {
    if (!obj || !data.second || !data.second->extraLists) {
      continue;  // Skip if object or data is null
    }
    auto uid = GetUniqueID(refr, data.second.get(), false);
    for (const auto* extraList : *data.second->extraLists) {
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
                                              Save::Types::UniqueID uid) {
  for (auto inventoryData = refr->GetInventory();
       auto& [obj, data] : inventoryData) {
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

inline RE::BIPED_OBJECT GetWeaponSlot(RE::TESObjectWEAP* weap) {
  if (!weap) {
    return RE::BIPED_OBJECT::kNone;
  }
#define WEAP_CASE(_TYPE)       \
  case RE::WEAPON_TYPE::_TYPE: \
    return RE::BIPED_OBJECT::_TYPE;
  switch (weap->GetWeaponType()) {
    WEAP_CASE(kBow)
    WEAP_CASE(kCrossbow)
    WEAP_CASE(kHandToHandMelee)
    WEAP_CASE(kOneHandAxe)
    WEAP_CASE(kOneHandDagger)
    WEAP_CASE(kOneHandMace)
    WEAP_CASE(kOneHandSword)
    WEAP_CASE(kStaff)
    case RE::WEAPON_TYPE::kTwoHandAxe:
    case RE::WEAPON_TYPE::kTwoHandSword:
      return RE::BIPED_OBJECT::kTwoHandMelee;
    default:
      return RE::BIPED_OBJECT::kNone;
  }
#undef WEAP_CASE
}

inline void ResetDisplayName(const RE::InventoryEntryData* data) {
  if (data && data->extraLists) {
    for (auto& extraList : *data->extraLists) {
      if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
        extraList->RemoveByType(RE::ExtraDataType::kTextDisplayData);
        return;
      }
    }
  }
}

inline void SetItemDisplayName(RE::InventoryEntryData* data, const char* name) {
  if (auto* front = GetOrCreateExtraList(data); data && front) {
    if (auto* textDisplayData = front->GetByType<RE::ExtraTextDisplayData>()) {
      textDisplayData->SetName(name);
    } else {
      textDisplayData = new RE::ExtraTextDisplayData(name);
      front->Add(textDisplayData);
    }
  }
}
}  // namespace Helpers