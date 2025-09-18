#pragma once

#include <glaze/glaze.hpp>
#include "Types.h"
#include "Helpers.h"

namespace Save::V1 {
struct SaveData {
  static constexpr u8 VERSION = 1;
  std::vector<Types::ArmorRecordEntryV1> armorRecords{};
  Types::UniqueIDHistoryV1 uniqueIDHistory{};
  u8 version{VERSION};

  void Read(SKSE::SerializationInterface* iface, uint32_t type,
            uint32_t length) {
    _DEBUG("Reading record: type={}, version={}, length={}", type, 1, length);
    if (type == Types::SaveDataHeaderV1::IDENTIFIER) {
      return;  // Header record, nothing to do here
    }
    if (type == Types::ArmorRecordEntryV1::IDENTIFIER) {
      _DEBUG("Reading saved armor records");
      if (!Helpers::ReadJsonObject(iface, armorRecords)) {
        _ERROR("Failed to read armor records from serialization interface");
        return;
      }
      _DEBUG("Deserialized {} armor records", armorRecords.size());
    }
    if (type == Types::UniqueIDHistoryV1::IDENTIFIER) {
      _DEBUG("Reading unique ID history");
      if (!iface->ReadRecordData(uniqueIDHistory.nextUID)) {
        _ERROR("Failed to read unique ID nextUID from serialization interface");
        return;
      }
      if (!Helpers::ReadJsonObject(iface, uniqueIDHistory.rows)) {
        _ERROR("Failed to read unique ID rows from serialization interface");
        return;
      }
      _DEBUG("Deserialized {} unique ID rows",
             std::distance(uniqueIDHistory.rows.begin(),
                           uniqueIDHistory.rows.end()));
    }
  }

  void Write(SKSE::SerializationInterface* iface) const {
    _DEBUG("Writing {} armor records", armorRecords.size());
    if (!iface->OpenRecord(Types::ArmorRecordEntryV1::IDENTIFIER, 1)) {
      _ERROR("Failed to open record for ArmorRecordEntry");
      return;
    }
    if (!Helpers::WriteJsonObject(iface, armorRecords)) {
      _ERROR("Failed to write armor records to serialization interface");
      return;
    }

    if (!iface->OpenRecord(Types::UniqueIDHistoryV1::IDENTIFIER, 1)) {
      _ERROR("Failed to open record for UniqueIDHistory");
      return;
    }
    _DEBUG("Writing unique ID offset: {}", uniqueIDHistory.nextUID);
    if (!iface->WriteRecordData(uniqueIDHistory.nextUID)) {
      _ERROR("Failed to write unique ID nextUID to serialization interface");
      return;
    }
    if (!Helpers::WriteJsonObject(iface, uniqueIDHistory.rows)) {
      _ERROR("Failed to write unique ID rows to serialization interface");
      return;
    }
  }
};
}  // namespace Save::V1