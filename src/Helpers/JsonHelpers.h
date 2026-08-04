#pragma once
#include <glaze/glaze.hpp>

namespace JsonHelpers {
template <typename T>
std::optional<T> MaybeGet(const glz::generic& json, std::string_view str) {
  if (json.contains(str)) {
    return json[str].as<T>();
  }
  return std::nullopt;
}
}