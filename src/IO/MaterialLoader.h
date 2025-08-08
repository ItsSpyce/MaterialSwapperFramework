#pragma once

#include "Filesystem.h"
#include "MaterialConfig.h"
#include "Models/MaterialFileBase.h"


class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting);

  _NODISCARD static MaterialRecord* LoadMaterial(
      const std::string& filename);

  _NODISCARD static MaterialConfig* GetMaterialConfig(
      RE::FormID formID, const std::string& materialName);

  _NODISCARD static MaterialConfig* GetDefaultMaterial(RE::FormID formID);

  static void VisitMaterialFilesForFormID(
      uint32_t formID,
      const std::function<void(const MaterialConfig*)>& visitor);

 private:
  static inline std::unordered_map<uint32_t, std::vector<MaterialConfig>>
      materialConfigs_{};
};