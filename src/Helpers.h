#pragma once

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
  return std::stoul(name, nullptr, 16);
}

struct InventoryItem {
  RE::TESBoundObject* object;
  int count;
  std::unique_ptr<RE::InventoryEntryData> data;
  int uid;
};

static void VisitEquippedInventoryItems(
    RE::TESObjectREFR* refr,
    const std::function<void(const std::unique_ptr<InventoryItem>&)>& visitor) {
  auto inventoryData = refr->GetInventory();
  for (auto& [obj, data] : inventoryData) {
    if (data.second->extraLists) {
      for (auto& extraList : *data.second->extraLists) {
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

static std::unique_ptr<InventoryItem> GetInventoryItemWithFormID(
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
}  // namespace Helpers