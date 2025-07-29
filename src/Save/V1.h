#pragma once

#include <glaze/glaze.hpp>

namespace Save::V1 {
struct SaveDataHeader {
  static constexpr uint32_t IDENTIFIER = 'HEAD';
};

struct ArmorRecordEntry {
  static constexpr uint32_t IDENTIFIER = 'ARMO';
  int uid;
  std::vector<std::string> appliedMaterials;
};

struct SaveData {
  SaveDataHeader header{};
  std::vector<ArmorRecordEntry> armorRecords{};

  void Read(SKSE::SerializationInterface* iface, uint32_t type,
            uint32_t length) {
    logger::debug("Reading record: type={}, version={}, length={}", type, 1,
                  length);
    if (type == SaveDataHeader::IDENTIFIER) {
      return;  // Header record, nothing to do here
    }
    if (type == ArmorRecordEntry::IDENTIFIER) {
      logger::debug("Reading saved armor records");
      size_t armorJsonLength;
      if (!iface->ReadRecordData(armorJsonLength)) {
        logger::error("Failed to read armor records size from serialization interface");
        return;
      }
      std::string armorJson(armorJsonLength, '\0');
      if (!iface->ReadRecordData(armorJson.data(), armorJsonLength)) {
        logger::error("Failed to read armor records data from serialization interface");
        return;
      }
      logger::debug("Deserializing armor records from JSON: {}", armorJson);
      std::vector<ArmorRecordEntry> records;
      if (auto err = glz::read_json(records, armorJson)) {
        auto cleanedError = glz::format_error(err);
        logger::error("Failed to read armor records from JSON: {}", cleanedError);
        return;
      }
      logger::debug("Deserialized {} armor records", records.size());
      armorRecords = records;
    }
  }

  void Write(SKSE::SerializationInterface* iface) const {
    logger::debug("Writing {} armor records", armorRecords.size());
    if (!iface->OpenRecord(ArmorRecordEntry::IDENTIFIER, 1)) {
      logger::error("Failed to open record for ArmorRecordEntry");
      return;
    }
    // my original idea wasn't working, so now I'm just gonna use glaze to serialize :)
    std::string armorJson{};
    if (auto err = glz::write_json(armorRecords, armorJson)) {
      auto cleanedError = glz::format_error(err);
      logger::error("Failed to write armor records to JSON: {}", cleanedError);
      return;
    }
    logger::debug("Serialized armor records to JSON: {}", armorJson);
    if (!iface->WriteRecordData(armorJson.size())) {
      logger::error("Failed to write armor records size to serialization interface");
      return;
    }
    if (!iface->WriteRecordData(armorJson.data(), armorJson.size())) {
      logger::error("Failed to write armor records data to serialization interface");
      return;
    }
  }
};
}  // namespace Save::V1