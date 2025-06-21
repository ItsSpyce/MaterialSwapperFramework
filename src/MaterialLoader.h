#pragma once

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "MaterialFileReader.h"
#include "StringHelpers.h"
#include "ShaderMaterialFile.h"

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
        for (const auto& material : entry) {
          if (!material.filename.empty()) {
            auto filenameHash = hash(material.filename);
            filenameHashes[filenameHash] = material.filename;
          }
        }
      }
    }
  }

  _NODISCARD static std::shared_ptr<ShaderMaterialFile> LoadMaterial(
      const MaterialRecord& materialConfig) {
    auto path = fs::path("Data") / materialConfig.filename;
    return LoadMaterialFromDisk(path.string());
  }

  _NODISCARD static std::shared_ptr<ShaderMaterialFile> LoadMaterial(
      size_t filenameHash) {
    auto it = filenameHashes.find(filenameHash);
    if (it != filenameHashes.end()) {
      return LoadMaterialFromDisk(it->second);
    }
    logger::warn("No material file found for hash: {}", filenameHash);
    return nullptr;
  }

  _NODISCARD static std::shared_ptr<ShaderMaterialFile> LoadMaterial(
      const std::string& filename) {
    if (filename.empty()) {
      logger::error("Filename is empty");
      return nullptr;
    }
    auto path = fs::path("Data") / filename;
    return LoadMaterialFromDisk(path.string());
  }

  _NODISCARD static MaterialRecord& GetMaterial(const size_t filenameHash) {
    if (const auto it = filenameHashes.find(filenameHash);
        it != filenameHashes.end()) {
      auto lowered = StringHelpers::ToLower(it->second);
      for (auto& records : materialConfigs_ | std::views::values) {
        for (auto& record : records) {
          if (StringHelpers::ToLower(record.filename) == lowered) {
            return record;
          }
        }
      }
    }
    throw std::runtime_error(
        fmt::format("No material found for hash: {}", filenameHash));
  }

  _NODISCARD static MaterialRecord& GetMaterial(RE::FormID formID, const std::string& materialName) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (auto& record : it->second) {
        if (StringHelpers::ToLower(record.name) == StringHelpers::ToLower(materialName)) {
          return record;
        }
      }
    }
    throw std::runtime_error(
        fmt::format("No material found for form ID: {} and name: {}", formID,
                    materialName));
  }

  _NODISCARD static MaterialRecord& GetDefaultMaterial(RE::FormID formID) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (auto& record : it->second) {
        if (!record.modifyName) {
          return record;
        }
      }
    }
    throw std::runtime_error(
        fmt::format("No default material found for form ID: {}", formID));
  }

  static void VisitMaterialFilesForFormID(
      uint32_t formID,
      const std::function<void(const MaterialRecord&)>& visitor) {
    auto it = materialConfigs_.find(formID);
    if (it != materialConfigs_.end()) {
      for (const auto& entry : it->second) {
        visitor(entry);
      }
    } else {
      logger::warn("No material config found for form ID: {}", formID);
    }
  }

 private:
  static std::shared_ptr<ShaderMaterialFile> LoadMaterialFromDisk(
      const std::string& filename) {
    if (!fs::exists(filename)) {
      logger::warn("Material file does not exist: {}", filename);
      return nullptr;
    }
    MaterialFileReader reader;
    auto stream = std::make_unique<std::ifstream>(filename, std::ios::binary);
    if (!stream->is_open()) {
      throw std::runtime_error(
          fmt::format("Failed to open file: {}", filename));
    }
    stream->exceptions(std::ios::badbit);
    reader.read(std::move(stream));
    auto file = reader.file();
    // TODO: load templated materials
    return file;
  }

  static inline std::unordered_map<uint32_t, std::vector<MaterialRecord>>
      materialConfigs_{};
  static inline std::unordered_map<size_t, std::string> filenameHashes{};
};