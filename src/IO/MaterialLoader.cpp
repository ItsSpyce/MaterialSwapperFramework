#include "MaterialLoader.h"

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "Helpers/SkyrimHelpers.h"
#include "Helpers/StringHelpers.h"
#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"

// Merge parent into child, preferring child's values
#define MERGE_FIELD(_FIELD) \
  if (!record._FIELD.has_value()) record._FIELD = parent._FIELD

static bool LoadMaterialFromDisk(const string& filename,
                                 MaterialRecord& record) noexcept {
  if (filename.empty() || !fs::exists(filename)) {
    _ERROR("Material file does not exist: {}", filename);
    return false;
  }
  if (!StringHelpers::ToLower(filename).ends_with(".json")) {
    _ERROR("JSON file expected, but got: {}", filename);
    return false;
  }
  if (auto err = glz::read_file_jsonc<glz::opts{
          .error_on_unknown_keys = false, .error_on_missing_keys = false}>(
          record, filename, string{})) {
    auto cleanedError = glz::format_error(err);
    _ERROR("Failed to read material file {}: {}", filename, cleanedError);
    return false;
  }

  // load templated materials
  if (record.inherits.has_value()) {
    MaterialRecord parent;
    const auto fullPath =
        Filesystem::Join("Data", "materials", record.inherits.value());
    if (!LoadMaterialFromDisk(fullPath.string(), parent)) {
      _ERROR("Failed to load parent material: {}", record.inherits.value());
      return false;
    }

    // TODO: merge texture

    MERGE_FIELD(clamp);
    MERGE_FIELD(uvOffset);
    MERGE_FIELD(uvScale);
    MERGE_FIELD(transparency);
    MERGE_FIELD(alphaBlend);
    MERGE_FIELD(sourceBlendMode);
    MERGE_FIELD(destinationBlendMode);
    MERGE_FIELD(alphaTestThreshold);
    MERGE_FIELD(alphaTest);
    MERGE_FIELD(refractionPower);
    MERGE_FIELD(rimPower);
    MERGE_FIELD(specularPower);
    MERGE_FIELD(specularColor);
    MERGE_FIELD(specularMult);
    MERGE_FIELD(emitColor);
    MERGE_FIELD(emitMult);

    MERGE_FIELD(colorBlendMap);
    MERGE_FIELD(colorBlendMode);
    MERGE_FIELD(colorChannelR);
    MERGE_FIELD(colorChannelG);
    MERGE_FIELD(colorChannelB);
  }
  return true;
}

void MaterialLoader::ReadMaterialsFromDisk(bool clearExisting) {
  if (clearExisting) {
    materialConfigs_.clear();
  }
  for (auto& jsonFile : Filesystem::EnumerateMaterialConfigDir()) {
    _DEBUG("Reading material config file: {}", jsonFile.path().string());
    auto loweredPath = StringHelpers::ToLower(jsonFile.path().string());
    if (auto filename =
            StringHelpers::ToLower(jsonFile.path().filename().string());
        !filename.ends_with(".json") || filename[0] == '_') {
      continue;
    }
    MaterialConfigMap config;
    if (auto err =
            glz::read_file_json<glz::opts{.error_on_missing_keys = false}>(
                config, loweredPath, string{})) {
      auto cleanedError = glz::format_error(err);
      _ERROR("Failed to read material config file {}: {}", loweredPath,
             cleanedError);
      continue;
    }
    for (auto& [formID, entry] : config) {
      auto realFormID = Helpers::GetFormID(formID);
      if (realFormID == 0) {
        _ERROR("Invalid form ID in material config: {}", formID);
        continue;
      }
      auto storedMaterials = materialConfigs_.get_or_return_default(realFormID);
      for (auto& record : entry) {
        storedMaterials[record.name] = record;
      }
      materialConfigs_[realFormID] = storedMaterials;
    }
  }
  for (auto& modDirectory : Filesystem::EnumerateModsInMaterialDir()) {
    // the mod name is the last part of the path
    auto modName = modDirectory.path().filename().string();
    if (modName.empty() || modName[0] == '_') {
      continue;  // Skip empty or hidden directories
    }
    static constexpr auto IGNORE_DIRECTORIES = {"config", "shaders",
                                                "translations"};
    if (ranges::find(IGNORE_DIRECTORIES, StringHelpers::ToLower(modName)) !=
        IGNORE_DIRECTORIES.end()) {
      continue;  // Skip ignored directories
    }
    // format for a directory name is "MOD_NAME.es{m,p,l}"
    auto* plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(modName);
    if (!plugin) {
      _WARN("Plugin not loaded for mod directory: {}", modName);
      continue;  // Skip if the plugin is not loaded
    }
    for (auto& jsonFile : fs::recursive_directory_iterator(modDirectory)) {
      MaterialConfigMap config;
      if (!jsonFile.is_regular_file() || !jsonFile.path().has_extension() ||
          jsonFile.path().extension() != ".json") {
        continue;  // Skip non-JSON files
      }
      auto loweredPath = StringHelpers::ToLower(jsonFile.path().string());
      _DEBUG("Reading material config file: {}", loweredPath);
      if (auto err =
              glz::read_file_json<glz::opts{.error_on_missing_keys = false}>(
                  config, loweredPath, string{})) {
        auto cleanedError = glz::format_error(err);
        _ERROR("Failed to read material config file {}: {}", loweredPath,
               cleanedError);
        continue;
      }
      for (auto& [formID, entry] : config) {
        auto realFormID = Helpers::GetFormID(formID);
        if (realFormID == 0) {
          _ERROR("Invalid form ID in material config: {}", formID);
          continue;
        }

        auto storedMaterials =
            materialConfigs_.get_or_return_default(realFormID);
        for (auto& record : entry) {
          storedMaterials[record.name] = record;
        }
        materialConfigs_[realFormID] = storedMaterials;
      }
    }
  }
}

MaterialRecord* MaterialLoader::LoadMaterial(const string& filename) {
  if (filename.empty()) {
    _ERROR("Filename is empty");
    return nullptr;
  }
  auto path = fs::path("Data") / "Materials" / filename;
  static emhash8::HashMap<string, MaterialRecord> materialCache;
  if (auto it = materialCache.find(path.string()); it != materialCache.end()) {
    return &it->second;  // Return cached record
  }
  MaterialRecord record;
  if (!LoadMaterialFromDisk(path.string(), record)) {
    return nullptr;
  }
  // Store the record in the cache
  return &(materialCache[path.string()] = move(record));
}

MaterialConfig* MaterialLoader::GetMaterialConfig(RE::FormID formID,
                                                  const string& materialName) {
  auto* records = materialConfigs_.try_get(formID);
  if (!records) {
    _WARN("No material configs found for form ID: {:08X}", formID);
    return nullptr;
  }
  return records->try_get(materialName);
}

MaterialConfig* MaterialLoader::GetDefaultMaterial(RE::FormID formID) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second | views::values) {
      if (!record.modifyName) {
        return &record;
      }
    }
  }
  return nullptr;
}

void MaterialLoader::VisitMaterialFilesForFormID(
    u32 formID, const Visitor<const MaterialConfig&>& visitor) {
  auto materialConfigs = materialConfigs_.get_or_return_default(formID);
  for (const auto& val : materialConfigs | views::values) {
    BREAK_IF_STOP(visitor, val);
  }
}