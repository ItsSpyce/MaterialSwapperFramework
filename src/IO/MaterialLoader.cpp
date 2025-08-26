#include "MaterialLoader.h"

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "Helpers.h"
#include "MaterialConfig.h"
#include "Models/MaterialRecord.h"
#include "StringHelpers.h"

static bool LoadMaterialFromDisk(const std::string& filename,
                                 MaterialRecord& record) {
  if (filename.empty() || !fs::exists(filename)) {
    logger::error("Material file does not exist: {}", filename);
    return false;
  }
  if (!StringHelpers::ToLower(filename).ends_with(".json")) {
    logger::error("JSON file expected, but got: {}", filename);
    return false;
  }
  if (auto err = glz::read_file_json<glz::opts{.error_on_unknown_keys = false,
                                               .new_lines_in_arrays = true}>(
          record, filename, std::string{})) {
    auto cleanedError = glz::format_error(err);
    logger::error("Failed to read material file {}: {}", filename,
                  cleanedError);
    return false;
  }
  // TODO: load templated materials
  return true;
}

void MaterialLoader::ReadMaterialsFromDisk(bool clearExisting) {
  if (clearExisting) {
    materialConfigs_.clear();
  }
  for (auto& jsonFile : Filesystem::EnumerateMaterialConfigDir()) {
    auto loweredPath = StringHelpers::ToLower(jsonFile.path().string());
    auto filename = StringHelpers::ToLower(jsonFile.path().filename().string());
    if (!filename.ends_with(".json") || filename[0] == '_') {
      continue;
    }
    logger::debug("Reading material config file: {}", loweredPath);
    MaterialConfigMap config;
    if (auto err = glz::read_file_json(config, loweredPath, std::string{})) {
      auto cleanedError = glz::format_error(err);
      logger::error("Failed to read material config file {}: {}", loweredPath,
                    cleanedError);
      continue;
    }
    for (auto& [formID, entry] : config) {
      auto realFormID = Helpers::GetFormID(formID);
      if (realFormID == 0) {
        logger::error("Invalid form ID in material config: {}", formID);
        continue;
      }
      if (auto it = materialConfigs_.find(realFormID);
          it != materialConfigs_.end()) {
        it->second.insert_range(it->second.end(), entry);
      } else {
        materialConfigs_[realFormID] = entry;
      }
    }
  }
  for (auto& modDirectory : Filesystem::EnumerateModsInMaterialDir()) {
    // the mod name is the last part of the path
    auto modName = modDirectory.path().filename().string();
    if (modName.empty() || modName[0] == '_') {
      continue;  // Skip empty or hidden directories
    }
    if (modName == "config") {
      continue;  // Skip the config directory
    }
    // format for a directory name is "MOD_NAME.es{m,p,l}"
    auto* plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(modName);
    if (!plugin) {
      logger::warn("Plugin not loaded for mod directory: {}", modName);
      continue;  // Skip if the plugin is not loaded
    }
    for (auto& jsonFile : fs::recursive_directory_iterator(modDirectory)) {
      MaterialConfigMap config;
      if (!jsonFile.is_regular_file() || !jsonFile.path().has_extension() ||
          jsonFile.path().extension() != ".json") {
        continue;  // Skip non-JSON files
      }
      auto loweredPath = StringHelpers::ToLower(jsonFile.path().string());
      logger::debug("Reading material config file: {}", loweredPath);
      if (auto err = glz::read_file_json(config, loweredPath, std::string{})) {
        auto cleanedError = glz::format_error(err);
        logger::error("Failed to read material config file {}: {}", loweredPath,
                      cleanedError);
        continue;
      }
      for (auto& [formID, entry] : config) {
        auto realFormID = Helpers::GetFormID(formID);
        if (realFormID == 0) {
          logger::error("Invalid form ID in material config: {}", formID);
          continue;
        }
        if (auto it = materialConfigs_.find(realFormID);
            it != materialConfigs_.end()) {
          it->second.insert_range(it->second.end(), entry);
        } else {
          materialConfigs_[realFormID] = entry;
        }
      }
    }
  }
}

MaterialRecord* MaterialLoader::LoadMaterial(const std::string& filename) {
  if (filename.empty()) {
    logger::error("Filename is empty");
    return nullptr;
  }
  auto path = fs::path("Data") / "Materials" / filename;
  static std::unordered_map<std::string, MaterialRecord> materialCache;
  if (auto it = materialCache.find(path.string()); it != materialCache.end()) {
    return &it->second;  // Return cached record
  }
  MaterialRecord record;
  if (!LoadMaterialFromDisk(path.string(), record)) {
    return nullptr;
  }
  // Store the record in the cache
  return &(materialCache[path.string()] = std::move(record));
}

MaterialConfig* MaterialLoader::GetMaterialConfig(
    RE::FormID formID, const std::string& materialName) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second) {
      if (StringHelpers::ToLower(record.name) ==
          StringHelpers::ToLower(materialName)) {
        // Create a unique_ptr by copying the found record
        return &record;
      }
    }
  } else {
    logger::warn("No material configs found for form ID: {:08X}", formID);
  }
  return nullptr;
}

MaterialConfig* MaterialLoader::GetDefaultMaterial(RE::FormID formID) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second) {
      if (!record.modifyName) {
        return &record;
      }
    }
  }
  return nullptr;
}

void MaterialLoader::VisitMaterialFilesForFormID(
    uint32_t formID,
    const Visitor<const MaterialConfig&>& visitor) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (const auto& entry : it->second) {
      BREAK_IF_STOP(visitor, entry);
    }
  }
}