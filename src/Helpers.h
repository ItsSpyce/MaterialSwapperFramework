#pragma once

#include "EditorIDCache.h"
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

inline u16 GetUniqueID(RE::TESObjectREFR* refr,
                       const RE::InventoryEntryData* data, bool init) {
  if (!refr || !data) {
    return 0;
  }
  auto* armo = data->object->As<RE::TESObjectARMO>();
  if (!armo) {
    return 0;
  }
  string editorID(EditorIDCache::GetEditorID(armo->GetFormID()));
  if (editorID.empty()) {
    _WARN("Armor has no editor ID for form: {}", armo->GetFormID());
    return 0;
  }
  return UniqueIDTable::GetSingleton()->GetUID(refr, editorID, init);
}

inline u16 GetUniqueID(RE::TESObjectREFR* refr,
                       RE::BGSBipedObjectForm::BipedObjectSlot slot,
                       bool init) {
  if (!refr && !init) {
    return 0;
  }
  auto* actor = refr->As<RE::Actor>();
  if (!actor) {
    return 0;
  }
  if (auto* armo = actor->GetWornArmor(slot)) {
    string editorID(EditorIDCache::GetEditorID(armo->GetFormID()));
    if (editorID.empty()) {
      _WARN("Armor has no editor ID for slot: {}", static_cast<int>(slot));
      return 0;
    }
    return UniqueIDTable::GetSingleton()->GetUID(refr, editorID, init);
  }
  _WARN("No armor found for slot: {}", static_cast<int>(slot));
  return 0;
}

inline RE::TESForm* GetFormFromUniqueID(u16 uid) {
  // form ID is stored in the high 16 bits
  auto editorID = UniqueIDTable::GetSingleton()->GetEditorID(uid);
  if (editorID.empty()) {
    return nullptr;
  }
  auto formID = EditorIDCache::GetFormID(editorID);
  if (formID == 0) {
    _WARN("Form ID not found for editor ID: {}", editorID);
    return nullptr;
  }
  return RE::TESForm::LookupByID(formID);
}

struct InventoryItem {
  RE::TESBoundObject* object;
  i32 count;
  std::unique_ptr<RE::InventoryEntryData> data;
  u16 uid;
};

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
                                                 u32 formID) {
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
                                              u16 uid) {
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
}  // namespace Helpers