#include "Save.h"

#include "Factories.h"
#include "UniqueIDTable.h"

constexpr uint32_t IDENTIFIER = 'MSF_';

void Save::SaveCallback(SKSE::SerializationInterface* iface) {
  _INFO("Saving data...");
  V1::SaveData data;
  Factories::ArmorFactory::GetSingleton()->WriteToSave(data);
  UniqueIDTable::GetSingleton()->WriteToSave(data);
  data.Write(iface);
}

void Save::LoadCallback(SKSE::SerializationInterface* iface) {
  uint32_t type, version, length;
  _INFO("Reading saved data...");
  bool didReadSave = false;
  while (iface->GetNextRecordInfo(type, version, length)) {
    didReadSave = true;
    _INFO("Loading save data: type={}, version={}, length={}", type, version,
          length);
    if (version == V1::SaveData::VERSION) {
      V1::SaveData data;
      data.Read(iface, type, length);
      Factories::ArmorFactory::GetSingleton()->ReadFromSave(data);
      UniqueIDTable::GetSingleton()->ReadFromSave(data);
    } else {
      _ERROR("Unknown version: {}", version);
    }
  }
  if (didReadSave) {
    _INFO("Finished loading save");
  } else {
    logger::warn("No save data found, nothing to load");
  }
}

void Save::RevertCallback(SKSE::SerializationInterface* iface) {
  // Handle revert logic if necessary
}

bool Save::Install() {
  auto iface = SKSE::GetSerializationInterface();
  if (!iface) {
    _ERROR("Failed to get serialization interface");
    return false;
  }
  iface->SetUniqueID(IDENTIFIER);
  iface->SetSaveCallback(SaveCallback);
  iface->SetLoadCallback(LoadCallback);
  iface->SetRevertCallback(RevertCallback);

  return true;
}