#include "Save.h"

#include "Factories.h"
#include "UniqueIDTable.h"

constexpr uint32_t IDENTIFIER = 'MSF_';
static list<ISaveable*> saveables;

void Save::SaveCallback(SKSE::SerializationInterface* iface) {
  _INFO("Saving data...");
  SaveData data;
  for (const auto& saveable : saveables) {
    saveable->WriteToSave(iface, data);
  }
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
      RE::DebugMessageBox(
          "Material Swapper Framework\nExisting save is incompatible with the "
          "latest version. Previously saved configurations will be lost due to "
          "incompatibility. This message box will disappear when you resave.");
    } else if (version == V2::SaveData::VERSION) {
      data.Read(iface, type, length);
      for (const auto& saveable : saveables) {
        saveable->ReadFromSave(iface, data);
      }
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

  saveables.push_back(Factories::ArmorFactory::GetSingleton());
  saveables.push_back(UniqueIDTable::GetSingleton());

  return true;
}