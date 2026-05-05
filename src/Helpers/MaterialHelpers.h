#pragma once

namespace MaterialHelpers {
inline RE::NiColor* GetColorPtr(const array<float, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return new RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                           colorArr[2] / 255.0f);
  }
  return new RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

inline RE::NiColorA* GetColorAPtr(const array<float, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return new RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                            colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return new RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

inline RE::NiColor GetColor(const array<float, 3>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f) {
    return RE::NiColor(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                       colorArr[2] / 255.0f);
  }
  return RE::NiColor(colorArr[0], colorArr[1], colorArr[2]);
}

inline RE::NiColorA GetColorA(const array<float, 4>& colorArr) {
  if (colorArr[0] > 1.0f || colorArr[1] > 1.0f || colorArr[2] > 1.0f ||
      colorArr[3] > 1.0f) {
    return RE::NiColorA(colorArr[0] / 255.0f, colorArr[1] / 255.0f,
                        colorArr[2] / 255.0f, colorArr[3] / 255.0f);
  }
  return RE::NiColorA(colorArr[0], colorArr[1], colorArr[2], colorArr[3]);
}

inline RE::NiPoint2 GetPoint2(const array<float, 2>& pointArr) {
  return RE::NiPoint2(pointArr[0], pointArr[1]);
}

inline RE::NiPoint3 GetPoint3(const array<float, 3>& pointArr) {
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
}  // namespace MaterialHelpers