#include "MaterialLoader.h"

#include <glaze/glaze.hpp>

#include "Filesystem.h"
#include "Helpers.h"
#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"
#include "StringHelpers.h"

// Merge parent into child, preferring child's values
#define MERGE_FIELD(_FIELD) \
  if (!record._FIELD.has_value()) record._FIELD = parent._FIELD

static bool LoadMaterialFromDisk(const std::string& filename,
                                 MaterialRecord& record) {
  if (filename.empty() || !fs::exists(filename)) {
    logger::error("Material file does not exist: {}", filename);
    return false;
  }
  if (!StringHelpers::ToLower(filename).ends_with(".json")) {
    logger::error("JSON file expected, but got: {}", filename);
    return false;
  }
  if (auto err = glz::read_file_json<glz::opts{.error_on_unknown_keys = false,
                                               .new_lines_in_arrays = true}>(
          record, filename, std::string{})) {
    auto cleanedError = glz::format_error(err);
    logger::error("Failed to read material file {}: {}", filename,
                  cleanedError);
    return false;
  }
  // load templated materials
  if (record.inherits.has_value()) {
    MaterialRecord parent;
    if (!LoadMaterialFromDisk(record.inherits.value(), parent)) {
      logger::error("Failed to load parent material: {}",
                    record.inherits.value());
      return false;
    }
    
    MERGE_FIELD(shaderType);
    MERGE_FIELD(clamp);
    MERGE_FIELD(uvOffset);
    MERGE_FIELD(uvScale);
    MERGE_FIELD(transparency);
    MERGE_FIELD(alphaBlend);
    MERGE_FIELD(sourceBlendMode);
    MERGE_FIELD(destinationBlendMode);
    MERGE_FIELD(alphaTestThreshold);
    MERGE_FIELD(alphaTest);
    MERGE_FIELD(depthWrite);
    MERGE_FIELD(depthTest);
    MERGE_FIELD(ssr);
    MERGE_FIELD(wetnessControlSsr);
    MERGE_FIELD(decal);
    MERGE_FIELD(twoSided);
    MERGE_FIELD(decalNoFade);
    MERGE_FIELD(nonOccluder);
    MERGE_FIELD(refraction);
    MERGE_FIELD(refractionalFalloff);
    MERGE_FIELD(refractionPower);
    MERGE_FIELD(envMapEnabled);
    MERGE_FIELD(envMapMaskScale);
    MERGE_FIELD(depthBias);
    MERGE_FIELD(grayscaleToPaletteColor);
    MERGE_FIELD(maskWrites);
    // BGSM specific fields
    MERGE_FIELD(diffuseMap);
    MERGE_FIELD(normalMap);
    MERGE_FIELD(smoothSpecMap);
    MERGE_FIELD(grayscaleMap);
    MERGE_FIELD(glowMap);
    MERGE_FIELD(wrinkleMap);
    MERGE_FIELD(specularMap);
    MERGE_FIELD(lightingMap);
    MERGE_FIELD(flowMap);
    MERGE_FIELD(distanceFieldAlphaMap);
    MERGE_FIELD(envMap);
    MERGE_FIELD(innerLayerMap);
    MERGE_FIELD(displacementMap);
    MERGE_FIELD(baseMap);
    MERGE_FIELD(envMapMask);
    MERGE_FIELD(enableEditorAlphaThreshold);
    MERGE_FIELD(translucency);
    MERGE_FIELD(translucencyThickObject);
    MERGE_FIELD(translucencyMixAlbedoWithSubsurfaceColor);
    MERGE_FIELD(translucencySubsurfaceColor);
    MERGE_FIELD(translucencyTransmissiveScale);
    MERGE_FIELD(translucencyTurbulence);
    MERGE_FIELD(rimLighting);
    MERGE_FIELD(rimPower);
    MERGE_FIELD(backLightPower);
    MERGE_FIELD(specularPower);
    MERGE_FIELD(subsurfaceLighting);
    MERGE_FIELD(subsurfaceLightingRolloff);
    MERGE_FIELD(specularEnabled);
    MERGE_FIELD(specularColor);
    MERGE_FIELD(specularMult);
    MERGE_FIELD(smoothness);
    MERGE_FIELD(fresnelPower);
    MERGE_FIELD(wetnessControlSpecScale);
    MERGE_FIELD(wetnessControlSpecPowerScale);
    MERGE_FIELD(wetnessControlSpecMinvar);
    MERGE_FIELD(wetnessControlEnvMapScale);
    MERGE_FIELD(wetnessControlFresnelPower);
    MERGE_FIELD(wetnessControlMetalness);
    MERGE_FIELD(pbr);
    MERGE_FIELD(customPorosity);
    MERGE_FIELD(porosityValue);
    MERGE_FIELD(rootMaterialPath);
    MERGE_FIELD(anisoLighting);
    MERGE_FIELD(emitEnabled);
    MERGE_FIELD(emitColor);
    MERGE_FIELD(emitMult);
    MERGE_FIELD(modelSpaceNormals);
    MERGE_FIELD(externalEmit);
    MERGE_FIELD(lumEmit);
    MERGE_FIELD(useAdaptiveEmissive);
    MERGE_FIELD(adaptiveEmissiveExposureParams);
    MERGE_FIELD(backLighting);
    MERGE_FIELD(receiveShadows);
    MERGE_FIELD(hideSecret);
    MERGE_FIELD(castShadows);
    MERGE_FIELD(dissolveFade);
    MERGE_FIELD(assumeShadowmask);
    MERGE_FIELD(glowMapEnabled);
    MERGE_FIELD(envMapWindow);
    MERGE_FIELD(envMapEye);
    MERGE_FIELD(hair);
    MERGE_FIELD(hairTintColor);
    MERGE_FIELD(tree);
    MERGE_FIELD(facegen);
    MERGE_FIELD(skinTint);
    MERGE_FIELD(tessellate);
    MERGE_FIELD(displacementMapParams);
    MERGE_FIELD(tessellationParams);
    MERGE_FIELD(grayscaleToPaletteScale);
    MERGE_FIELD(skewSpecularAlpha);
    MERGE_FIELD(terrain);
    MERGE_FIELD(terrainParams);

    MERGE_FIELD(blood);
    MERGE_FIELD(effectLighting);
    MERGE_FIELD(falloff);
    MERGE_FIELD(falloffColor);
    MERGE_FIELD(grayscaleToPaletteAlpha);
    MERGE_FIELD(soft);
    MERGE_FIELD(baseColor);
    MERGE_FIELD(baseColorScale);
    MERGE_FIELD(falloffParams);
    MERGE_FIELD(lightingInfluence);
    MERGE_FIELD(envMapMinLod);
    MERGE_FIELD(softDepth);
    MERGE_FIELD(effectPbrSpecular);
    MERGE_FIELD(color);
    MERGE_FIELD(colorBlendMode);
    MERGE_FIELD(colorBlendMap);
  }
  return true;
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
    static constexpr auto IGNORE_DIRECTORIES = {"config", "shaders",
                                                "translations"};
    if (std::ranges::find(IGNORE_DIRECTORIES,
                          StringHelpers::ToLower(modName)) !=
        IGNORE_DIRECTORIES.end()) {
      continue;  // Skip ignored directories
    }
    // format for a directory name is "MOD_NAME.es{m,p,l}"
    auto* plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(modName);
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

MaterialRecord* MaterialLoader::LoadMaterial(const std::string& filename) {
  if (filename.empty()) {
    logger::error("Filename is empty");
    return nullptr;
  }
  auto path = fs::path("Data") / "Materials" / filename;
  static std::unordered_map<std::string, MaterialRecord> materialCache;
  if (auto it = materialCache.find(path.string()); it != materialCache.end()) {
    return &it->second;  // Return cached record
  }
  MaterialRecord record;
  if (!LoadMaterialFromDisk(path.string(), record)) {
    return nullptr;
  }
  // Store the record in the cache
  return &(materialCache[path.string()] = std::move(record));
}

MaterialConfig* MaterialLoader::GetMaterialConfig(
    RE::FormID formID, const std::string& materialName) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second) {
      if (StringHelpers::ToLower(record.name) ==
          StringHelpers::ToLower(materialName)) {
        // Create a unique_ptr by copying the found record
        return &record;
      }
    }
  } else {
    logger::warn("No material configs found for form ID: {:08X}", formID);
  }
  return nullptr;
}

MaterialConfig* MaterialLoader::GetDefaultMaterial(RE::FormID formID) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (auto& record : it->second) {
      if (!record.modifyName) {
        return &record;
      }
    }
  }
  return nullptr;
}

void MaterialLoader::VisitMaterialFilesForFormID(
    uint32_t formID, const Visitor<const MaterialConfig&>& visitor) {
  if (auto it = materialConfigs_.find(formID); it != materialConfigs_.end()) {
    for (const auto& entry : it->second) {
      BREAK_IF_STOP(visitor, entry);
    }
  }
}