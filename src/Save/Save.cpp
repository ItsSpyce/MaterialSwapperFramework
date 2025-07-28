#include "Save.h"

#include "Factories.h"
#include "V1.h"

constexpr uint32_t IDENTIFIER = 'MSF_';

void Save::SaveCallback(SKSE::SerializationInterface* iface) {
  logger::info("Saving data...");
  V1::SaveData data;
  Factories::ArmorFactory::GetSingleton()->WriteToSave(data);
  data.Write(iface);
}

void Save::LoadCallback(SKSE::SerializationInterface* iface) {
  uint32_t type, version, length;
  logger::info("Reading saved data...");
  bool didReadSave = false;
  while (iface->GetNextRecordInfo(type, version, length)) {
    didReadSave = true;
    logger::info("Loading save data: type={}, version={}, length={}", type,
                 version, length);
    if (version == 1) {
      V1::SaveData data;
      data.Read(iface, type, length);
      Factories::ArmorFactory::GetSingleton()->LoadFromSave(data);
    } else {
      logger::error("Unknown version: {}", version);
    }
  }
  if (didReadSave) {
    logger::info("Finished loading save");
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
    logger::error("Failed to get serialization interface");
    return false;
  }
  iface->SetUniqueID(IDENTIFIER);
  iface->SetSaveCallback(SaveCallback);
  iface->SetLoadCallback(LoadCallback);
  iface->SetRevertCallback(RevertCallback);

  return true;
}