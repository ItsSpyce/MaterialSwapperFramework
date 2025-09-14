#pragma once

struct MaterialRecord;
struct MaterialConfig;

class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting);

  _NODISCARD static MaterialRecord* LoadMaterial(
      const std::string& filename);

  _NODISCARD static MaterialConfig* GetMaterialConfig(
      RE::FormID formID, const std::string& materialName);

  _NODISCARD static MaterialConfig* GetDefaultMaterial(RE::FormID formID);

  _NODISCARD static bool HasMaterialConfigs(RE::FormID formID) {
    return materialConfigs_.contains(formID);
  }

  static void VisitMaterialFilesForFormID(
      uint32_t formID,
      const Visitor<const MaterialConfig&>& visitor);

 private:
  static inline std::unordered_map<uint32_t, std::vector<MaterialConfig>>
      materialConfigs_{};
};