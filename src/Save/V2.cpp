#include "V2.h"

#include "Helpers.h"

void Save::V2::SaveData::Read(SKSE::SerializationInterface* iface, u32 type, u32 length) {
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
  if (type == Types::NPCRecordEntryV2::IDENTIFIER) {
    if (!Helpers::ReadJsonObject(iface, npcRecords)) {
      _ERROR("Failed to read NPC records from serialization interface");
      return;
    }
  }
  if (type == Types::WeaponRecordEntryV2::IDENTIFIER) {
    if (!Helpers::ReadJsonObject(iface, weaponRecords)) {
      _ERROR("Failed to read weapon records from serialization interface");
      return;
    }
  }
}

void Save::V2::SaveData::Write(SKSE::SerializationInterface* iface) const {
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
  if (!iface->OpenRecord(Types::NPCRecordEntryV2::IDENTIFIER, 2)) {
    _ERROR("Failed to open record for NPCRecordEntry");
    return;
  }
  if (!Helpers::WriteJsonObject(iface, npcRecords)) {
    _ERROR("Failed to write NPC records to serialization interface");
    return;
  }
  if (!iface->OpenRecord(Types::WeaponRecordEntryV2::IDENTIFIER, 2)) {
    _ERROR("Failed to open record for WeaponRecordEntry");
    return;
  }
  if (!Helpers::WriteJsonObject(iface, weaponRecords)) {
    _ERROR("Failed to write weapon records to serialization interface");
    return;
  }
}