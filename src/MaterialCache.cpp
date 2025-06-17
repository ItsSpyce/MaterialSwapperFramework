#include "MaterialCache.h"

#include <glaze/glaze.hpp>

#include "MaterialLoader.h"

constexpr glz::opts options{.minified = true};

void MaterialCache::SaveCallback(SKSE::SerializationInterface* serialization) {
  std::string buffer;
  auto err = glz::write_json(cacheMap_, buffer);
  if (err) {
    auto cleanedError = glz::format_error(err);
    logger::error("Failed to serialize material cache: {}", cleanedError);
  }
}

void MaterialCache::LoadCallback(SKSE::SerializationInterface* serialization) {
  std::string data;
  uint32_t dataLen = 0;
  uint32_t type = 0;
  uint32_t version = 0;
  uint32_t length = 0;
  while (serialization->GetNextRecordInfo(type, version, length)) {
    if (type == 'LEN_') {
      if (!serialization->ReadRecordData(&dataLen, sizeof(uint32_t))) {
        logger::error("Failed to read material cache data length");
        return;
      }
    }
    if (type == 'DAT_') {
      if (!serialization->ReadRecordData(&data, length)) {
        logger::error("Failed to read material cache data");
        return;
      }
    }
  }
  if (dataLen > 0) {
    if (const auto err = glz::read<options>(cacheMap_, data)) {
      auto cleanedError = glz::format_error(err);
      logger::error("Failed to deserialize material cache: {}", cleanedError);
    }
  }
  logger::info("Completed load");
}

void MaterialCache::Initialize() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  const auto serialization = SKSE::GetSerializationInterface();
  serialization->SetUniqueID('MATS');
  serialization->SetSaveCallback(SaveCallback);
  serialization->SetLoadCallback(LoadCallback);
}

std::shared_ptr<ShaderMaterialFile> MaterialCache::GetMaterial(
    const int32_t uid) {
  if (const auto it = cacheMap_.find(uid); it != cacheMap_.end()) {
    return MaterialLoader::LoadMaterial(it->second);
  }
  return nullptr;
}
