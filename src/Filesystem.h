#pragma once

namespace fs = std::filesystem;

namespace Filesystem {
static constinit inline auto MATERIAL_FILES_DIR = R"(Data\Materials)";
static constinit inline auto MATERIAL_PLUGIN_DIR =
    R"(Data\SKSE\Plugins\MaterialSwapperFramework)";
static constinit inline auto MATERIAL_SHADER_DIR =
    R"(Data\SKSE\Plugins\MaterialSwapperFramework\shaders)";
static constinit inline auto MATERIAL_CONFIG_DIR =
    R"(Data\SKSE\Plugins\MaterialSwapperFramework\config)";

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