#pragma once

#include "Types.h"

namespace Save::V1 {
class SaveData {
 public:
  static constexpr u8 VERSION = 1;
  std::vector<Types::ArmorRecordEntryV1> armorRecords{};
  Types::UniqueIDHistoryV1 uniqueIDHistory{};
  u8 version{VERSION};

  void Read(SKSE::SerializationInterface* iface, uint32_t type,
            uint32_t length);

  void Write(SKSE::SerializationInterface* iface) const;
};
}  // namespace Save::V1