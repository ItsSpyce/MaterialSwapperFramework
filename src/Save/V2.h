#pragma once

#include "Types.h"

namespace Save::V1 {
class SaveData;
}

namespace Save::V2 {

class SaveData : public Types::IMigrate<V1::SaveData> {
 public:
  static constexpr u8 VERSION = 2;
  std::unordered_map<RE::FormID, std::vector<Types::ArmorRecordEntryV2>> armorRecords{};
  std::unordered_map<RE::FormID, std::vector<Types::WeaponRecordEntryV2>> weaponRecords{};
  Types::UniqueIDHistoryV2 uniqueIDHistory{};
  std::vector<Types::NPCRecordEntryV2> npcRecords{};
  u8 version{VERSION};

  void Read(SKSE::SerializationInterface* iface, uint32_t type,
            uint32_t length);

  void Write(SKSE::SerializationInterface* iface) const;

  bool Migrate(const V1::SaveData&) override { return false; }
};
}  // namespace Save::V2