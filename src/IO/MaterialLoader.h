#pragma once

struct MaterialRecord;
struct MaterialConfig;

class MaterialLoader {
 public:
  void static ReadMaterialsFromDisk(bool clearExisting);

  _NODISCARD static MaterialRecord* LoadMaterial(const string& filename);

  _NODISCARD static MaterialConfig* GetMaterialConfig(
      RE::FormID formID, const string& materialName);

  _NODISCARD static MaterialConfig* GetDefaultMaterial(RE::FormID formID);

  _NODISCARD static bool HasMaterialConfigs(RE::FormID formID) {
    return materialConfigs_.contains(formID);
  }

  static void VisitMaterialFilesForFormID(
      u32 formID, const Visitor<const MaterialConfig&>& visitor);

 private:
  static inline emhash8::HashMap<u32, emhash8::HashMap<string, MaterialConfig>>
      materialConfigs_{};
};