#pragma once

#include "EditorIDCache.h"
#include "NiOverride.h"

namespace Helpers {

inline int GetModIndex(std::string_view name) {
  auto esp = RE::TESDataHandler::GetSingleton()->LookupModByName(name);
  if (!esp) return -1;
  return !esp->IsLight() ? esp->compileIndex << 24
                         : (0xFE000 | esp->smallFileCompileIndex) << 12;
}

inline uint32_t GetFormID(const std::string& name) {
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

struct InventoryItem {
  RE::TESBoundObject* object;
  int count;
  std::unique_ptr<RE::InventoryEntryData> data;
  int uid;
};

inline void VisitEquippedInventoryItems(
    RE::TESObjectREFR* refr, const Visitor<std::unique_ptr<InventoryItem>&>& visitor) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (data.second->extraLists) {
      for (auto& extraList : *data.second->extraLists) {
        if (!extraList) {
          continue;
        }
        if (extraList->HasType(RE::ExtraDataType::kWorn)) {
          auto* armo = obj->As<RE::TESObjectARMO>();
          // auto* weap = obj->As<RE::TESObjectWEAP>(); // TODO
          auto uid = NiOverride::GetItemUniqueID()(
              RE::StaticFunctionTag{}, refr, 0,
              armo ? static_cast<int>(armo->GetSlotMask()) : 0, false);
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
}

inline std::unique_ptr<InventoryItem> GetInventoryItemWithFormID(
    RE::TESObjectREFR* refr, uint32_t formID) {
  auto inventoryData = refr->GetInventory(
      [&](const RE::TESBoundObject& obj) { return obj.GetFormID() == formID; });
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto* armo = obj->As<RE::TESObjectARMO>();
    auto uid = NiOverride::GetItemUniqueID()(
        RE::StaticFunctionTag{}, refr, 0,
        armo ? static_cast<int>(armo->GetSlotMask()) : 0, false);

    return std::make_unique<InventoryItem>(
        InventoryItem{.object = obj,
                      .count = data.first,
                      .data = std::move(data.second),
                      .uid = uid});
  }
  return std::unique_ptr<InventoryItem>{};
}

inline std::unique_ptr<InventoryItem> GetInventoryItemWithUID(
    RE::TESObjectREFR* refr, int uid) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (!obj || !data.second) {
      continue;  // Skip if object or data is null
    }
    auto* armo = obj->As<RE::TESObjectARMO>();
    auto itemUID = NiOverride::GetItemUniqueID()(
        RE::StaticFunctionTag{}, refr, 0,
        armo ? static_cast<int>(armo->GetSlotMask()) : 0, false);
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