#pragma once

namespace StringHelpers {
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

inline std::string ToLower(std::string_view str) {
  std::string lowerStr(str);
  std::ranges::transform(lowerStr, lowerStr.begin(), ::tolower);
  return lowerStr;
}

inline std::string PrefixTexturesPath(const std::string_view& path) {
  return fmt::format("textures\\{}", path);
}

inline bool HasMaterialName(std::string_view name, size_t& index) {
  index = name.find(" (*");
  return index != std::string::npos;
}

inline bool HasMaterialName(RE::TESForm* form, size_t& index) {
  if (!form) {
    return false;
  }
  return HasMaterialName(form->GetName(), index);
}

inline void Join(const std::vector<const char*>& strings,
                        const char* delimiter, std::string& out) {
  if (strings.empty()) {
    return;
  }
  std::string result;
  for (const auto& str : strings) {
    if (!result.empty()) {
      result += delimiter;
    }
    result += str;
  }
  out = std::move(result);
}
}  // namespace StringHelpers