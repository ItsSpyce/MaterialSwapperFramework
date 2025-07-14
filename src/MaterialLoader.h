#pragma once

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "Models/MaterialFileBase.h"
#include "StringHelpers.h"

namespace fs = std::filesystem;

class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting) {
    if (clearExisting) {
      materialConfigs_.clear();
    }
    for (auto& jsonFile : Filesystem::EnumerateMaterialConfigDir()) {
      auto loweredPath = StringHelpers::ToLower(jsonFile.path().string());
      auto filename =
          StringHelpers::ToLower(jsonFile.path().filename().string());
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
  }

  _NODISCARD static std::shared_ptr<MaterialRecord> LoadMaterial(
      const std::string& filename) {
    if (filename.empty()) {
      logger::error("Filename is empty");
      return nullptr;
    }
    auto path = fs::path("Data") / "Materials" / filename;
    return LoadMaterialFromDisk(path.string());
  }

  _NODISCARD static MaterialConfig* GetMaterial(
      RE::FormID formID, const std::string& materialName) {
    if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
      for (auto& record : it->second) {
        if (StringHelpers::ToLower(record.name) ==
            StringHelpers::ToLower(materialName)) {
          return &record;
        }
      }
    }
    return nullptr;
  }

  _NODISCARD static MaterialConfig* GetDefaultMaterial(RE::FormID formID) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (auto& record : it->second) {
        if (!record.modifyName) {
          return &record;
        }
      }
    }
    return nullptr;
  }

  static void VisitMaterialFilesForFormID(
      uint32_t formID,
      const std::function<void(const MaterialConfig&)>& visitor) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (const auto& entry : it->second) {
        visitor(entry);
      }
    }
  }

 private:
  static std::shared_ptr<MaterialRecord> LoadMaterialFromDisk(
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
    return std::make_shared<MaterialRecord>(std::move(record));
  }

  static inline std::unordered_map<uint32_t, std::vector<MaterialConfig>>
      materialConfigs_{};
};