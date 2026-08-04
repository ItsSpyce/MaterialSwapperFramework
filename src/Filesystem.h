#pragma once

namespace fs = std::filesystem;

namespace Filesystem {
static constexpr std::string MATERIAL_FILES_DIR = "Data/Materials";
static constexpr std::string SKSE_PLUGINS_DIR = "Data/SKSE/Plugins";
static constexpr std::string MATERIAL_PLUGIN_DIR =
    SKSE_PLUGINS_DIR + "/MaterialSwapperFramework";
static constexpr std::string MATERIAL_SHADER_DIR =
    MATERIAL_PLUGIN_DIR + "/shaders";
static constexpr std::string MATERIAL_CACHE_DIR =
    MATERIAL_PLUGIN_DIR + "/cache";
static constexpr std::string MATERIAL_CONFIG_DIR =
    MATERIAL_PLUGIN_DIR + "/config";
static constexpr std::string MATERIAL_CONDITIONS_DIR =
    MATERIAL_PLUGIN_DIR + "/_conditions";
static constexpr std::string TEXTURES_DIR = "Data/Textures";
static constexpr std::string FILE_ID_BIN = MATERIAL_CACHE_DIR + "/file_ids.bin";

inline fs::recursive_directory_iterator EnumerateMaterialFilesDir() {
  if (!fs::exists(MATERIAL_FILES_DIR) ||
      !fs::is_directory(MATERIAL_FILES_DIR)) {
    return fs::recursive_directory_iterator{};
  }
  fs::recursive_directory_iterator it(MATERIAL_FILES_DIR);
  const fs::recursive_directory_iterator end;
  return it != end ? it : fs::recursive_directory_iterator{};
}

inline fs::recursive_directory_iterator EnumerateMaterialConfigDir() {
  if (!fs::exists(MATERIAL_CONFIG_DIR) ||
      !fs::is_directory(MATERIAL_CONFIG_DIR)) {
    return fs::recursive_directory_iterator{};
  }
  fs::recursive_directory_iterator it(MATERIAL_CONFIG_DIR);
  const fs::recursive_directory_iterator end;
  return it != end ? it : fs::recursive_directory_iterator{};
}

inline fs::directory_iterator EnumerateModsInMaterialDir() {
  if (!fs::exists(MATERIAL_PLUGIN_DIR) ||
      !fs::is_directory(MATERIAL_FILES_DIR)) {
    return fs::directory_iterator{};
  }
  fs::directory_iterator it(MATERIAL_PLUGIN_DIR);
  const fs::directory_iterator end;
  return it != end ? it : fs::directory_iterator{};
}

template <typename... Paths>
fs::path Join(Paths&&... paths) {
  return (fs::path(std::forward<Paths>(paths)) / ...);
}
}  // namespace Filesystem
