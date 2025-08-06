#include "MaterialLoader.h"

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "Models/MaterialFileBase.h"
#include "StringHelpers.h"

static std::unique_ptr<MaterialRecord> LoadMaterialFromDisk(
    const std::string& filename) {
  if (filename.empty() || !fs::exists(filename)) {
    logger::error("Material file does not exist: {}", filename);
    return nullptr;
  }
  if (!StringHelpers::ToLower(filename).ends_with(".json")) {
    logger::error("JSON file expected, but got: {}", filename);
    return nullptr;
  }
  MaterialRecord record{};
  if (auto err = glz::read_file_json<glz::opts{.error_on_unknown_keys = false,
                                               .new_lines_in_arrays = true}>(
          record, filename, std::string{})) {
    auto cleanedError = glz::format_error(err);
    logger::error("Failed to read material file {}: {}", filename,
                  cleanedError);
    return nullptr;
  }
  // TODO: load templated materials
  return std::make_unique<MaterialRecord>(std::move(record));
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
    auto* plugin =
        RE::TESDataHandler::GetSingleton()->LookupModByName(modName);
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

std::unique_ptr<MaterialRecord> MaterialLoader::LoadMaterial(
    const std::string& filename) {
  if (filename.empty()) {
    logger::error("Filename is empty");
    return nullptr;
  }
  auto path = fs::path("Data") / "Materials" / filename;
  return LoadMaterialFromDisk(path.string());
}

std::unique_ptr<MaterialConfig> MaterialLoader::GetMaterialConfig(
    RE::FormID formID, const std::string& materialName) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second) {
      if (StringHelpers::ToLower(record.name) ==
          StringHelpers::ToLower(materialName)) {
        // Create a unique_ptr by copying the found record
        return std::make_unique<MaterialConfig>(record);
      }
    }
  }
  return nullptr;
}

std::unique_ptr<MaterialConfig> MaterialLoader::GetDefaultMaterial(
    RE::FormID formID) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second) {
      if (!record.modifyName) {
        return std::make_unique<MaterialConfig>(record);
      }
    }
  }
  return nullptr;
}

void MaterialLoader::VisitMaterialFilesForFormID(
    uint32_t formID,
    const std::function<void(const std::unique_ptr<MaterialConfig>&)>&
        visitor) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (const auto& entry : it->second) {
      visitor(std::make_unique<MaterialConfig>(entry));
    }
  }
}