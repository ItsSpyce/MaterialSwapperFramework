#pragma once

#include "MathTypes.h"
#include "NiOverride.h"
#include "StringHelpers.h"

namespace Helpers {

using BipedObjectSlot = RE::BIPED_OBJECTS::BIPED_OBJECT;
static constexpr auto VALID_SLOTS = {
    BipedObjectSlot::kHead,
    BipedObjectSlot::kHair,
    BipedObjectSlot::kBody,
    BipedObjectSlot::kHands,
    BipedObjectSlot::kForearms,
    BipedObjectSlot::kAmulet,
    BipedObjectSlot::kRing,
    BipedObjectSlot::kFeet,
    BipedObjectSlot::kCalves,
    BipedObjectSlot::kShield,
    BipedObjectSlot::kTail,
    BipedObjectSlot::kLongHair,
    BipedObjectSlot::kCirclet,
    BipedObjectSlot::kEars,
    BipedObjectSlot::kModMouth,
    BipedObjectSlot::kModNeck,
    BipedObjectSlot::kModChestPrimary,
    BipedObjectSlot::kModBack,
    BipedObjectSlot::kModMisc1,
    BipedObjectSlot::kModPelvisPrimary,
    BipedObjectSlot::kDecapitateHead,
    BipedObjectSlot::kDecapitate,
    BipedObjectSlot::kModPelvisSecondary,
    BipedObjectSlot::kModLegRight,
    BipedObjectSlot::kModLegLeft,
    BipedObjectSlot::kModFaceJewelry,
    BipedObjectSlot::kModChestSecondary,
    BipedObjectSlot::kModShoulder,
    BipedObjectSlot::kModArmLeft,
    BipedObjectSlot::kModArmRight,
    BipedObjectSlot::kModMisc2,
    BipedObjectSlot::kFX01,
};

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

// thanks, Boost
template <class T>
inline void HashCombine(std::size_t& s, const T& v) {
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

inline void HashCombine(std::size_t& s, const Vector2& vec2) {
  HashCombine(s, vec2.x);
  HashCombine(s, vec2.y);
}

inline void HashCombine(std::size_t& s, const Vector3& vec3) {
  HashCombine(s, vec3.x);
  HashCombine(s, vec3.y);
  HashCombine(s, vec3.z);
}

inline void HashCombine(std::size_t& s, const Vector4& vec4) {
  HashCombine(s, vec4.x);
  HashCombine(s, vec4.y);
  HashCombine(s, vec4.z);
  HashCombine(s, vec4.w);
}

static void VisitArmorSlot(
    const std::function<void(RE::BIPED_OBJECTS::BIPED_OBJECT,
                             const std::string&)>& callback) {
  for (const auto slot : VALID_SLOTS) {
    if (callback) {
      callback(slot, StringHelpers::GetSlotName(slot));
    }
  }
}

struct InventoryItem {
  RE::TESBoundObject* object;
  int count;
  std::unique_ptr<RE::InventoryEntryData> data;
  int uid;
};

static void VisitEquippedInventoryItems(
    RE::TESObjectREFR* refr,
    const std::function<void(const InventoryItem&)>& visitor) {
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
          visitor({.object = obj,
                   .count = data.first,
                   .data = std::move(data.second),
                   .uid = uid});
        }
      }
    }
  }
}

static InventoryItem GetInventoryItemWithFormID(RE::TESObjectREFR* refr,
                                                uint32_t formID) {
  auto inventoryData = refr->GetInventory(
      [&](RE::TESBoundObject& obj) { return obj.GetFormID() == formID; });
  for (auto& [obj, data] : inventoryData) {
    return {.object = obj, .count = data.first, .data = std::move(data.second)};
  }
  return InventoryItem{};
}
}  // namespace Helpers