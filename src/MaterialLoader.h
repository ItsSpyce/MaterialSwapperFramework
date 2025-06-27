#pragma once

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "IO/BinaryMaterialReader.h"
#include "IO/JsonMaterialReader.h"
#include "StringHelpers.h"
#include "Models/MaterialFileBase.h"

namespace fs = std::filesystem;

class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting) {
    if (clearExisting) {
      materialConfigs_.clear();
    }
    std::hash<std::string> hash{};
    for (auto& jsonFile : Filesystem::EnumerateMaterialConfigDir()) {
      auto loweredPath = StringHelpers::ToLower(jsonFile.path().string());
      if (!loweredPath.ends_with(".json")) {
        continue;
      }
      logger::debug("Reading material config file: {}", loweredPath);
      MaterialConfig config;
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

  _NODISCARD static std::shared_ptr<MaterialFileBase> LoadMaterial(
      const std::string& filename) {
    if (filename.empty()) {
      logger::error("Filename is empty");
      return nullptr;
    }
    auto path = fs::path("Data") / filename;
    return LoadMaterialFromDisk(path.string());
  }

  _NODISCARD static MaterialRecord* GetMaterial(RE::FormID formID, const std::string& materialName) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (auto& record : it->second) {
        if (StringHelpers::ToLower(record.name) == StringHelpers::ToLower(materialName)) {
          return &record;
        }
      }
    }
    return nullptr;
  }

  _NODISCARD static MaterialRecord* GetDefaultMaterial(RE::FormID formID) {
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
      const std::function<void(const MaterialRecord&)>& visitor) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (const auto& entry : it->second) {
        visitor(entry);
      }
    }
  }

 private:
  static std::shared_ptr<MaterialFileBase> LoadMaterialFromDisk(
      const std::string& filename) {
    const auto ext = StringHelpers::ToLower(fs::path(filename).extension().string());
    IO::MaterialReaderBase* reader = nullptr;
    if (ext == ".json") {
      reader = new IO::JsonMaterialReader();
    } else if (ext == ".bgsm" || ext == ".bgem") {
      reader = new IO::BinaryMaterialReader();
    } else {
      logger::error("Unsupported material file extension: {}", ext);
      return nullptr;
    }
    reader->Read(filename);
    auto file = reader->File();
    // TODO: load templated materials
    return file;
  }

  static inline std::unordered_map<uint32_t, std::vector<MaterialRecord>>
      materialConfigs_{};
};