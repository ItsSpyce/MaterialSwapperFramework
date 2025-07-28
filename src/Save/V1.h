#pragma once

namespace Save::V1 {
struct SaveDataHeader {
  static constexpr uint32_t IDENTIFIER = 'HEAD';
};

struct ArmorRecordEntry {
  static constexpr uint32_t IDENTIFIER = 'ARMO';
  int uid;
  std::vector<RE::BSFixedString> appliedMaterials;
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
      uint32_t armorRecordCount;
      iface->ReadRecordData(&armorRecordCount, sizeof(armorRecordCount));
      logger::debug("Armor record count: {}", armorRecordCount);
      if (armorRecordCount > 0) {
        armorRecords.clear();
        armorRecords.resize(armorRecordCount);
        for (std::size_t i = 0; i < armorRecordCount; ++i) {
          ArmorRecordEntry entry;
          if (!iface->ReadRecordData(&entry.uid, sizeof(entry.uid))) {
            logger::error("Failed to read ArmorRecordEntry uid");
            continue;  // Skip this record if reading failed
          }
          std::size_t appliedMaterialsCount;
          iface->ReadRecordData(&appliedMaterialsCount,
                                sizeof(appliedMaterialsCount));
          logger::debug("Reading {} applied materials for uid: {}",
                        appliedMaterialsCount, entry.uid);
          for (std::size_t j = 0; j < appliedMaterialsCount; ++j) {
            // Read the length of the material name
            uint8_t materialLength;
            iface->ReadRecordData(&materialLength, sizeof(materialLength));
            if (materialLength == 0) {
              logger::warn("Empty material length found for uid: {}",
                           entry.uid);
              continue;  // Skip empty materials
            }
            logger::debug("Material length: {}", materialLength);
            std::string material(materialLength, '\0');
            iface->ReadRecordData(&material, material.size());
            if (!material.empty()) {
              entry.appliedMaterials.emplace_back(material);
              logger::debug("Read applied material: {}", material);
            } else {
              logger::warn("Empty material name found for uid: {}", entry.uid);
            }
          }
          armorRecords.emplace_back(entry);
        }
      }
    }
  }

  void Write(SKSE::SerializationInterface* iface) const {
    logger::debug("Writing {} armor records", armorRecords.size());
    if (!iface->OpenRecord(ArmorRecordEntry::IDENTIFIER, 1)) {
      logger::error("Failed to open record for ArmorRecordEntry");
      return;
    }
    uint32_t armorRecordCount = static_cast<uint32_t>(armorRecords.size());
    if (!iface->WriteRecordData(&armorRecordCount, sizeof(armorRecordCount))) {
      logger::error("Failed to write armor record count");
      return;
    }
    for (const auto& record : armorRecords) {
      if (!iface->WriteRecordData(&record.uid, sizeof(record.uid))) {
        logger::error("Failed to write ArmorRecordEntry uid");
        return;
      }
      std::size_t appliedMaterialsCount = record.appliedMaterials.size();
      if (!iface->WriteRecordData(&appliedMaterialsCount,
                                  sizeof(appliedMaterialsCount))) {
        logger::error(
            "Failed to write ArmorRecordEntry applied materials count");
        return;
      }
      // Write the applied materials array
      logger::debug("Writing {} applied materials for uid: {}",
                    record.appliedMaterials.size(), record.uid);
      for (const auto& material : record.appliedMaterials) {
        logger::debug("Writing applied material: {}", material);
        if (!iface->WriteRecordData((uint8_t)material.size())) {
          logger::error(
              "Failed to write ArmorRecordEntry applied material length");
          return;
        }
        if (!iface->WriteRecordData(material.c_str(), material.length() + 1)) {
          logger::error("Failed to write ArmorRecordEntry applied material: {}",
                        material);
          return;
        }
      }
    }
  }
};
}  // namespace Save::V1