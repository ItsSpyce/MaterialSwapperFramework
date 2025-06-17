#pragma once

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "MaterialFileReader.h"
#include "NifHelpers.h"
#include "ShaderMaterialFile.h"

namespace fs = std::filesystem;

class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting) {
    if (clearExisting) {
      materialFiles_.clear();
    }
    for (auto& jsonFile : Filesystem::EnumerateMaterialConfigDir()) {
      auto loweredPath = Helpers::ToLower(jsonFile.path().string());
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
      for (auto& [formID, armorConfig] : config.armors) {
        auto realFormID = Helpers::GetFormID(formID);
        for (auto& material : armorConfig.materials) {
          auto materialName = Helpers::ToLower(material.name);
          if (materialName.empty()) {
            continue;
          }
          auto materialFilePath = fs::path("Data") / material.filename;
          if (!fs::exists(materialFilePath)) {
            logger::error("Material file does not exist: {}",
                          materialFilePath.string());
            continue;
          }
          auto loweredMaterialFilePath =
              Helpers::ToLower(materialFilePath.string());
          materialFiles_.push_back(loweredMaterialFilePath);
          formIdToMaterialFileCache_[realFormID].push_back(
              materialFiles_.size() - 1);
        }
      }
    }

    logger::debug("Found {} material files", materialFiles_.size());
  }

  _NODISCARD static std::shared_ptr<ShaderMaterialFile> LoadMaterial(
      const std::string& name) {
    auto lowered = Helpers::ToLower(name);
    auto realPathName = lowered.starts_with("data\\")
                            ? lowered
                            : fs::path("data\\materials") / lowered;
    if (const auto it = std::ranges::find(materialFiles_, realPathName);
        it != materialFiles_.end()) {
      MaterialFileReader reader;
      auto filename = fs::path(realPathName);
      auto stream = std::make_unique<std::ifstream>(filename, std::ios::binary);
      if (!stream->is_open()) {
        throw std::runtime_error(
            fmt::format("Failed to open file: {}", filename.string()));
      }
      stream->exceptions(std::ios::badbit);
      reader.read(std::move(stream));
      return reader.file();
    }
    return nullptr;
  }

  _NODISCARD static std::shared_ptr<ShaderMaterialFile> LoadMaterial(
      const int32_t loaderHashCode) {
    if (const auto it = formIdToMaterialFileCache_.find(loaderHashCode);
        it != formIdToMaterialFileCache_.end()) {
      if (const auto& materialFileIndices = it->second;
          !materialFileIndices.empty()) {
        return LoadMaterial(materialFiles_[materialFileIndices[0]]);
      }
    }
    return nullptr;
  }

  static void VisitMaterialFiles(
      const std::function<void(const std::string&)>& visitor) {
    for (const auto& materialFile : materialFiles_) {
      visitor(materialFile);
    }
  }

  static void VisitMaterialFilesForFormID(uint32_t formID, const std::function<void(const std::string&)>& visitor) {
    if (const auto it = formIdToMaterialFileCache_.find(formID);
        it != formIdToMaterialFileCache_.end()) {
      for (const auto& index : it->second) {
        visitor(materialFiles_[index]);
      }
    }
  }

 private:
  static inline std::vector<std::string> materialFiles_;
  static inline std::unordered_map<uint32_t, std::vector<ptrdiff_t>>
      formIdToMaterialFileCache_;
};