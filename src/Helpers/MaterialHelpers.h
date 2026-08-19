#pragma once

namespace MaterialHelpers {
template <typename T>
  requires(std::is_arithmetic_v<T>)
RE::NiColor* GetColorPtr(const std::array<T, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return new RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                           colorArr[2] / 255.0f);
  }
  return new RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

template <typename T>
  requires(std::is_arithmetic_v<T>)
RE::NiColorA* GetColorAPtr(const std::array<T, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return new RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                            colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return new RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

template <typename T>
  requires(std::is_arithmetic_v<T>)
RE::NiColor GetColor(const std::array<T, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                       colorArr[2] / 255.0f);
  }
  return RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

template <typename T>
  requires(std::is_arithmetic_v<T>)
RE::NiColorA GetColorA(const std::array<T, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                        colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

template <typename T>
  requires(std::is_arithmetic_v<T>)
RE::NiPoint2 GetPoint2(const std::array<T, 2>& pointArr) {
  return RE::NiPoint2(pointArr[0], pointArr[1]);
}

template <typename T>
  requires(std::is_arithmetic_v<T>)
RE::NiPoint3 GetPoint3(const std::array<T, 3>& pointArr) {
  return RE::NiPoint3(pointArr[0], pointArr[1], pointArr[2]);
}

inline const char* GetStringPtr(const std::optional<std::string>& str) {
  if (str && !str->empty()) {
    return str->c_str();
  }
  return nullptr;
}

inline void CopyMembers(RE::BSShaderMaterial* from,
                        RE::BSLightingShaderMaterialBase* to) {
  if (auto* fromBase = skyrim_cast<RE::BSLightingShaderMaterialBase*>(from)) {
    to->CopyBaseMembers(fromBase);
  }
  to->CopyMembers(to);
}

inline const char* GetMaterialShapeKey(RE::FormID formID,
                                       const std::string& shapeName,
                                       const std::string& materialName) {
  return fmt::format("{}_{}_{}", formID, shapeName, materialName).c_str();
}
}  // namespace MaterialHelpers