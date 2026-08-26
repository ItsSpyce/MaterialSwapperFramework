#pragma once

#include "StringHelpers.h"

namespace NifHelpers {
template <typename T>
bool IsMaterialPath(T path) {
  auto str = StringHelpers::ToLower(path);
  return str.ends_with(".json");
}

inline std::optional<std::string> GetBuiltInMaterial(const RE::NiObjectNET* obj) {
  if (!obj) return std::nullopt;
  if (IsMaterialPath(obj->name)) return obj->name.c_str();
  return std::nullopt;
}

inline std::optional<std::string> GetBuiltInMaterial(const RE::NiPointer<RE::NiObjectNET>& obj) {
  return GetBuiltInMaterial(obj.get());
}
}  // namespace NifHelpers