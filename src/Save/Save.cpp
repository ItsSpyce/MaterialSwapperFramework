#include "Save.h"

#include "Factories.h"
#include "Translations.h"
#include "UniqueIDTable.h"

constexpr uint32_t IDENTIFIER = 'MSF_';
void Save::SaveCallback(SKSE::SerializationInterface* iface) {
  _INFO("Saving data...");
  SaveData data;
  UniqueIDTable::GetSingleton()->WriteToSave(iface, data);
  Factories::ArmorFactory::GetSingleton()->WriteToSave(iface, data);
  data.Write(iface);
}

void Save::LoadCallback(SKSE::SerializationInterface* iface) {
  uint32_t type, version, length;
  _INFO("Reading saved data...");
  bool didReadSave = false;
  SaveData data;
  while (iface->GetNextRecordInfo(type, version, length)) {
    didReadSave = true;
    _INFO("Loading save data: type={}, version={}, length={}", type, version,
          length);
    if (version == V1::SaveData::VERSION) {
      RE::DebugMessageBox(Translations::msfSaveDataIncompatibleWarning());
    } else if (version == V2::SaveData::VERSION) {
      data.Read(iface, type, length);
      UniqueIDTable::GetSingleton()->ReadFromSave(iface, data);
      Factories::ArmorFactory::GetSingleton()->ReadFromSave(iface, data);
    } else {
      _ERROR("Unknown version: {}", version);
    }
  }
  if (didReadSave) {
    _INFO("Finished loading save");
  } else {
    _WARN("No save data found, nothing to load");
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