#pragma once

#include <glaze/glaze.hpp>

namespace Save::V1 {
struct UniqueIDRow {
  size_t uid;
  RE::FormID ownerID;
  string editorID;
  mutable u32 savesSinceLastAccess;
};

struct SaveDataHeader {
  static constexpr uint32_t IDENTIFIER = 'HEAD';
};

struct ArmorRecordEntry {
  static constexpr uint32_t IDENTIFIER = 'ARMO';
  size_t uid;
  std::vector<std::string> appliedMaterials;
};

struct UniqueIDHistory {
  static constexpr uint32_t IDENTIFIER = 'UIDT';
  vector<UniqueIDRow> rows{};
  vector<size_t> freedUIDs{};
  size_t nextUID{1};
};

struct SaveData {
  static constexpr u8 VERSION = 1;
  std::vector<ArmorRecordEntry> armorRecords{};
  UniqueIDHistory uniqueIDHistory{};
  u8 version{VERSION};

  template <typename T>
  uint32_t ReadJsonObject(SKSE::SerializationInterface* iface, T& out) {
    size_t jsonLength;
    if (!iface->ReadRecordData(jsonLength)) {
      _ERROR("Failed to read JSON size from serialization interface");
      return 0;
    }
    string json(jsonLength, '\0');
    if (!iface->ReadRecordData(json.data(), jsonLength)) {
      _ERROR("Failed to read JSON data from serialization interface");
      return 0;
    }
    _DEBUG("Deserializing from JSON: {}", json);
    if (auto err = glz::read_json(out, json)) {
      auto cleanedError = glz::format_error(err);
      _ERROR("Failed to read object from JSON: {}", cleanedError);
      return 0;
    }
    return jsonLength;
  }

  template <typename T>
  bool WriteJsonObject(SKSE::SerializationInterface* iface,
                       const T& obj) const {
    string json{};
    if (auto err = glz::write_json(obj, json)) {
      auto cleanedError = glz::format_error(err);
      _ERROR("Failed to write object to JSON: {}", cleanedError);
      return false;
    }
    _DEBUG("Serialized to JSON: {}", json);
    if (!iface->WriteRecordData(json.size())) {
      _ERROR("Failed to write JSON size to serialization interface");
      return false;
    }
    if (!iface->WriteRecordData(json.data(), json.size())) {
      _ERROR("Failed to write JSON data to serialization interface");
      return false;
    }
    return true;
  }

  void Read(SKSE::SerializationInterface* iface, uint32_t type,
            uint32_t length) {
    _DEBUG("Reading record: type={}, version={}, length={}", type, 1, length);
    if (type == SaveDataHeader::IDENTIFIER) {
      return;  // Header record, nothing to do here
    }
    if (type == ArmorRecordEntry::IDENTIFIER) {
      _DEBUG("Reading saved armor records");
      if (!ReadJsonObject(iface, armorRecords)) {
        _ERROR("Failed to read armor records from serialization interface");
        return;
      }
      _DEBUG("Deserialized {} armor records", armorRecords.size());
    }
    if (type == UniqueIDHistory::IDENTIFIER) {
      _DEBUG("Reading unique ID history");
      if (!iface->ReadRecordData(uniqueIDHistory.nextUID)) {
        _ERROR("Failed to read unique ID nextUID from serialization interface");
        return;
      }
      if (!ReadJsonObject(iface, uniqueIDHistory.rows)) {
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
    if (!iface->OpenRecord(ArmorRecordEntry::IDENTIFIER, 1)) {
      _ERROR("Failed to open record for ArmorRecordEntry");
      return;
    }
    if (!WriteJsonObject(iface, armorRecords)) {
      _ERROR("Failed to write armor records to serialization interface");
      return;
    }

    if (!iface->OpenRecord(UniqueIDHistory::IDENTIFIER, 1)) {
      _ERROR("Failed to open record for UniqueIDHistory");
      return;
    }
    _DEBUG("Writing unique ID offset: {}", uniqueIDHistory.nextUID);
    if (!iface->WriteRecordData(uniqueIDHistory.nextUID)) {
      _ERROR("Failed to write unique ID nextUID to serialization interface");
      return;
    }
    if (!WriteJsonObject(iface, uniqueIDHistory.rows)) {
      _ERROR("Failed to write unique ID rows to serialization interface");
      return;
    }
  }
};
}  // namespace Save::V1