#pragma once

#include <glaze/glaze.hpp>

namespace Save::V2 {
/*
  New save format:
  armorRecords: Array<{
    uid: u16;
    appliedMaterials: Array<string>;
  }>,
  uniqueIDs: {
    freedUIDs: Array<{
      editorID: string;
      uid: u16;
    }>;
    rows: Map<string, {
      uid: u16,
      ownerID: u32,
      savesSinceLastAccess: u32,
      location: u8,
    }>;
  }
*/

struct ArmorRecordEntry {
  u16 uid;
  vector<string> appliedMaterials;
};

enum class UniqueItemLocation : u8 {
  kInventory = 0,
  kWorld = 1,
  kContainer = 2,
};

struct UniqueIDRow {
  u16 uid;
  RE::FormID ownerID;
  mutable u32 savesSinceLastAccess;
  UniqueItemLocation location;
};

struct FreedUIDEntry {
  string editorID;
  u16 uid;
};

struct UniqueIDHistory {
  vector<FreedUIDEntry> freedUIDs{};
  unordered_map<string, UniqueIDRow> rows{};
};

struct SaveData {
  static constexpr u8 VERSION = 2;
  vector<ArmorRecordEntry> armorRecords{};
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
};
}  // namespace Save::V2