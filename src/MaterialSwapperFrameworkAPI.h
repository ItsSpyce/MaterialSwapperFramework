#pragma once

#include <stdint.h>

namespace MSF_API {
constexpr const auto PluginName = "MaterialSwapperFramework";

enum class Version : std::uint8_t {
  v1,
};

enum class ApiResult : std::uint8_t {
  OK,
};
}