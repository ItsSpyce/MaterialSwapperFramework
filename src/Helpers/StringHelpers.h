#pragma once

#include "Core/StringReader.h"

namespace StringHelpers {
using namespace Core;
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

inline size_t GetPosForOneOf(const std::string& str, const char* chars) {
  for (size_t i = 0; i < strlen(chars); ++i) {
    const auto c = chars[i];
    if (auto pos = str.find(c); pos != std::string::npos) {
      return pos;
    }
  }
  return std::string::npos;
}

constexpr u32 GetStringHash(const char* str, size_t size) noexcept {
  u32 hash = 5381;
  for (const char* c = str; c < str + size; ++c) {
    hash = (hash << 5) + hash + (unsigned char)*c;
  }
  return hash;
}

constexpr u32 operator""_h(const char* str, size_t size) noexcept {
  return GetStringHash(str, size);
}

inline std::string ToLower(const std::string& str) noexcept {
  std::string lowerStr(str);
  std::ranges::transform(lowerStr, lowerStr.begin(), ::tolower);
  return lowerStr;
}

inline std::string operator""_l(const char* str) noexcept {
  return ToLower(str);
}

inline const char* GetNonNull(const std::initializer_list<const char*> strings) {
  for (auto& str : strings) {
    if (str != nullptr && strlen(str) > 0 && str[0] != '\0') return str;
  }
  return nullptr;
}

inline std::vector<std::string> Split(const std::string& str,
                                      const char delim) {
  std::vector<std::string> out;
  StringReader reader(str);
  while (!reader.AtEnd()) {
    out.emplace_back(reader.ReadUntil(delim));
    reader.Skip();
  }
  return std::move(out);
}

inline std::string Join(const std::vector<std::string>& strings,
                        const std::string& separator) {
  std::string result{};
  for (const auto& str : strings) {
    result += str + separator;
  }
  return result.substr(0, result.size() - separator.size());
}

inline std::string AssertPrefix(const std::string& str, const char* prefix) {
  if (strnicmp(str.c_str(), prefix, std::strlen(prefix)) == 0) {
    return str;
  }
  return std::string(prefix) + str;
}
}  // namespace StringHelpers