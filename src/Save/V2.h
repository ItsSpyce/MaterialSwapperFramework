#pragma once

#include "Helpers.h"
#include "Types.h"

namespace Save::V2 {

struct SaveData {
  static constexpr u8 VERSION = 2;
  unordered_map<RE::FormID, vector<Types::ArmorRecordEntryV2>> armorRecords{};
  Types::UniqueIDHistoryV2 uniqueIDHistory{};
  u8 version{VERSION};

  void Read(SKSE::SerializationInterface* iface, uint32_t type,
            uint32_t length) {
    _DEBUG("Reading record: type={}, version={}, length={}", type, 2, length);
    if (type == Types::ArmorRecordEntryV2::IDENTIFIER) {
      if (!Helpers::ReadJsonObject(iface, armorRecords)) {
        _ERROR("Failed to read armor records from serialization interface");
        return;
      }
    }
    if (type == Types::UniqueIDHistoryV2::IDENTIFIER) {
      if (!Helpers::ReadJsonObject(iface, uniqueIDHistory)) {
        _ERROR("Failed to read unique ID history from serialization interface");
        return;
      }
    }
  }

  void Write(SKSE::SerializationInterface* iface) const {
    if (!iface->OpenRecord(Types::ArmorRecordEntryV1::IDENTIFIER, 2)) {
      _ERROR("Failed to open record for ArmorRecordEntry");
      return;
    }
    if (!Helpers::WriteJsonObject(iface, armorRecords)) {
      _ERROR("Failed to write armor records to serialization interface");
      return;
    }
    if (!iface->OpenRecord(Types::UniqueIDHistoryV2::IDENTIFIER, 2)) {
      _ERROR("Failed to open record for UniqueIDHistory");
      return;
    }
    if (!Helpers::WriteJsonObject(iface, uniqueIDHistory)) {
      _ERROR("Failed to write unique ID history to serialization interface");
      return;
    }
  }
};
}  // namespace Save::V2