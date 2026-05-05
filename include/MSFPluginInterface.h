#pragma once

namespace MaterialSwapperFramework {
constexpr std::string_view PluginName = "MaterialSwapperFramework"sv;

enum class Version : std::uint8_t {
  v1,
};

enum ApiResult : std::uint8_t {
  OK = 0,
  _MAT_ERRORS = 100,
  MATERIAL_NOT_FOUND,
  INVALID_CONFIGURATION,
  INVALID_RECORD,
  _NIF_ERRORS = 200,
  MESH_NOT_FOUND,
};

struct IPluginInterface {};

}  // namespace MaterialSwapperFramework