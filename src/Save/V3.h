#pragma once

#include <emhash/hash_table8.hpp>
#include <srell.hpp>

#include "Types.h"
#include "Core/StringReader.h"
#include "Helpers/StringHelpers.h"

namespace Save::V3 {
using namespace Core;

class SaveData : public Types::IMigrate<V1::SaveData>,
                 public Types::IMigrate<V2::SaveData> {
public:
  static constexpr u8 VERSION = 3;
  emhash8::HashMap<UniqueID, std::vector<std::string>> entries;
  u8 version{VERSION};

  void Read(SKSE::SerializationInterface* iface, u32 type, u32) {
    static const auto parseRegex = srell::regex("\\(d+),(.*)|");
    if (type == 'V3') {
      size_t dataLen;
      if (!iface->ReadRecordData(dataLen)) {
        _ERROR("Failed to read save data");
        return;
      }
      if (dataLen == 0) {
        return;
      }
      std::string buffer(dataLen, '\0');
      if (iface->ReadRecordData(buffer.data(), dataLen) == 0u) {
        _ERROR("No read data found");
        return;
      }
      StringReader reader(buffer);
      while (!reader.AtEnd()) {
        auto uidStr = reader.ReadUntil(':');
        reader.Skip();
        auto materialsStr = reader.ReadUntil('|');
        reader.Skip();
        auto uid = static_cast<UniqueID>(std::stoi(uidStr));
        auto materials = StringHelpers::Split(materialsStr, ',');
        entries.emplace(uid, materials);
      }
    }
  }

  void Write(SKSE::SerializationInterface* iface) const {
    if (!iface->OpenRecord('V3', VERSION)) {
      _ERROR("Failed to open save records");
      return;
    }
    std::stringstream out;
    for (const auto& [uid, materials] : entries) {
      if (materials.empty()) continue;
      out << uid << ':';
      auto implodedStr = StringHelpers::Join(materials, ",");
      out << implodedStr << '|';
    }
    auto str = out.str();
    if (!iface->WriteRecordData(str.length())) {
      _ERROR("Failed to write string size");
      return;
    }
    if (!iface->WriteRecordData(out)) {
      _ERROR("Failed to write UID map");
    }
  }

  bool Migrate(const V1::SaveData&) override {
    return false;
  }

  bool Migrate(const V2::SaveData& from) override {
    for (const auto oldEntries : from.armorRecords | std::views::values) {
      for (const auto [uniqueID, appliedMaterials] : oldEntries) {
        entries[uniqueID] = appliedMaterials;
      }
    }
    return true;
  }
};
}  // namespace Save::V3