#pragma once

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "Models/MaterialFileBase.h"


class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting);

  _NODISCARD static std::unique_ptr<MaterialRecord> LoadMaterial(
      const std::string& filename);

  _NODISCARD static std::unique_ptr<MaterialConfig> GetMaterialConfig(
      RE::FormID formID, const std::string& materialName);

  _NODISCARD static std::unique_ptr<MaterialConfig> GetDefaultMaterial(RE::FormID formID);

  static void VisitMaterialFilesForFormID(
      uint32_t formID,
      const std::function<void(const std::unique_ptr<MaterialConfig>&)>& visitor);

 private:
  static inline std::unordered_map<uint32_t, std::vector<MaterialConfig>>
      materialConfigs_{};
};