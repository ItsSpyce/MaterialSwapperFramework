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

inline size_t GetUniqueID(RE::TESObjectREFR* refr,
                          const unique_ptr<RE::InventoryEntryData>& data,
                          bool init) {
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

inline size_t GetUniqueID(RE::TESObjectREFR* refr,
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

inline RE::TESForm* GetFormFromUniqueID(int uid) {
  // form ID is stored in the high 16 bits
  auto editorID = UniqueIDTable::GetSingleton()->GetEditorID(uid);
  if (editorID.empty()) {
    _WARN("Editor ID not found for unique ID: {}", uid);
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
  size_t uid;
};

inline void VisitEquippedInventoryItems(
    RE::TESObjectREFR* refr,
    const Visitor<std::unique_ptr<InventoryItem>&>& visitor) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second || !data.second->extraLists) {
      continue;  // Skip if object or data is null
    }
    auto uid = GetUniqueID(refr, data.second, false);
    for (auto* extraList : *data.second->extraLists) {
      if (!extraList) {
        continue;
      }
      if (extraList->HasType(RE::ExtraDataType::kWorn)) {
        auto inventoryItem = std::make_unique<InventoryItem>(
            InventoryItem{.object = obj,
                          .count = data.first,
                          .data = std::move(data.second),
                          .uid = uid});
        visitor(inventoryItem);
      }
    }
  }
}

inline std::unique_ptr<InventoryItem> GetInventoryItemWithFormID(
    RE::TESObjectREFR* refr, u32 formID) {
  auto inventoryData = refr->GetInventory(
      [&](const RE::TESBoundObject& obj) { return obj.GetFormID() == formID; });
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto uid = GetUniqueID(refr, data.second, false);

    return std::make_unique<InventoryItem>(
        InventoryItem{.object = obj,
                      .count = data.first,
                      .data = std::move(data.second),
                      .uid = uid});
  }
  return std::unique_ptr<InventoryItem>{};
}

inline std::unique_ptr<InventoryItem> GetInventoryItemWithUID(
    RE::TESObjectREFR* refr, u32 uid) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto itemUID = GetUniqueID(refr, data.second, false);
    if (itemUID == uid) {
      return std::make_unique<InventoryItem>(
          InventoryItem{.object = obj,
                        .count = data.first,
                        .data = std::move(data.second),
                        .uid = itemUID});
    }
  }
  return std::unique_ptr<InventoryItem>{};
}
}  // namespace Helpers