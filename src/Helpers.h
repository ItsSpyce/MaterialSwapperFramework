#pragma once
#include "MathTypes.h"

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

inline std::string ToLower(std::string_view str) {
  std::string lowerStr(str);
  std::ranges::transform(lowerStr, lowerStr.begin(), ::tolower);
  return lowerStr;
}

inline std::string PrefixTexturesPath(const std::string_view& path) {
  return fmt::format("textures\\{}", path);
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

inline const char* GetSlotName(const RE::BIPED_OBJECTS::BIPED_OBJECT slot) {
  switch (slot) {
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kHead:
      return "Head";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kHair:
      return "Hair";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kBody:
      return "Body";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kHands:
      return "Hands";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kForearms:
      return "Forearms";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kAmulet:
      return "Amulet";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kRing:
      return "Ring";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kFeet:
      return "Feet";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kCalves:
      return "Calves";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kShield:
      return "Shield";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kTail:
      return "Tail";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kLongHair:
      return "Long Hair";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kCirclet:
      return "Circlet";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kEars:
      return "Ears";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModMouth:
      return "Mouth";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModNeck:
      return "Neck";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModChestPrimary:
      return "Chest (Primary)";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModBack:
      return "Back";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModMisc1:
      return "Misc 1";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModPelvisPrimary:
      return "Pelvis (Primary)";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kDecapitateHead:
      return "Decapitate Head";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kDecapitate:
      return "Decapitate";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModPelvisSecondary:
      return "Pelvis (Secondary)";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModLegRight:
      return "Right Leg";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModLegLeft:
      return "Left Leg";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModFaceJewelry:
      return "Face Jewelry";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModChestSecondary:
      return "Chest (Secondary)";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModShoulder:
      return "Shoulder";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModArmLeft:
      return "Left Arm";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModArmRight:
      return "Right Arm";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kModMisc2:
      return "Misc 2";
    case RE::BIPED_OBJECTS::BIPED_OBJECT::kFX01:
      return "FX01";
    default:
      return "Unknown Slot";
  }
}

inline static void VisitArmorSlot(
    const std::function<void(RE::BIPED_OBJECTS::BIPED_OBJECT, const std::string&)>& callback) {
  for (const auto slot : VALID_SLOTS) {
    if (callback) {
      callback(slot, GetSlotName(slot));
    }
  }
}
}  // namespace Helpers