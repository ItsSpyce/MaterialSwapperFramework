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

template<>
inline std::optional<half> MaybeGet(const glz::generic& json, std::string_view str) {
  if (json.contains(str)) {
    return half{(float)json[str].get_number()};
  }
  return std::nullopt;
}

template<>
inline std::optional<u8> MaybeGet(const glz::generic& json, std::string_view str) {
  if (json.contains(str)) {
    return static_cast<u8>(std::min(json[str].get_number(), (double)UINT8_MAX));
  }
  return std::nullopt;
}

template<size_t Count, typename ArrayType>
std::optional<array<ArrayType, Count>> MaybeGetArray(const glz::generic& json, std::string_view str) {
  static_assert(Count > 0);
  if (json.contains(str)) {
    array<ArrayType, Count> result{};
    auto arr = json[str].get_array();
    for (size_t i = 0; i < Count; ++i) {
      result[i] = arr[i].get<ArrayType>();
    }
    return result;
  }
  return std::nullopt;
}

template<size_t Count>
std::optional<array<u8, Count>> MaybeGetArray(const glz::generic& json, std::string_view str) {
  static_assert(Count > 0);
  if (json.contains(str)) {
    array<u8, Count> result{};
    auto arr = json[str].get_array();
    for (size_t i = 0; i < Count; ++i) {
      result[i] = static_cast<u8>(std::min(arr[i].get_number(), (double)UINT8_MAX));
    }
    return result;
  }
  return std::nullopt;
}
}