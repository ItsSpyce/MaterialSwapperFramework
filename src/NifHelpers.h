#pragma once
#include "StringHelpers.h"

namespace NifHelpers {
template <typename T>
bool IsMaterialPath(T path) {
  auto str = StringHelpers::ToLower(path);
  return str.ends_with(".json");
}

inline bool HasBuiltInMaterial(RE::NiObjectNET* obj) {
  if (!obj) {
    return false;
  }
  if (IsMaterialPath(obj->name)) {
    return true;
  }
  return false;
}

inline bool HasBuiltInMaterial(const RE::NiPointer<RE::NiObjectNET>& obj) {
  return HasBuiltInMaterial(obj.get());
}

inline bool IsMaterialSwappable(RE::NiObjectNET* obj) {
  if (!obj) {
    return false;
  }
  if (obj->HasExtraData("DefaultMaterial")) {
    return true;
  }
  return HasBuiltInMaterial(obj);
}

inline bool IsMaterialSwappable(const RE::NiPointer<RE::NiObjectNET>& obj) {
  return IsMaterialSwappable(obj.get());
}

inline void VisitNiObject(RE::NiAVObject* obj,
                          const std::function<void(RE::NiAVObject*)>& visitor) {
  if (!obj) {
    return;
  }
  visitor(obj);
  if (const auto node = obj->AsNode()) {
    for (auto& child : node->children) {
      if (!child) {
        continue;
      }
      VisitNiObject(child.get(), visitor);
    }
  }
}

inline void VisitNiObject(const RE::NiPointer<RE::NiAVObject>& obj,
                          const std::function<void(RE::NiAVObject*)>& visitor) {
  VisitNiObject(obj.get(), visitor);
}

inline void VisitShapes(RE::NiAVObject* obj,
                        const std::function<void(RE::BSTriShape*)>& visitor) {
  if (!obj) {
    return;
  }
  VisitNiObject(obj, [&](RE::NiAVObject* avObj) {
    if (const auto triShape = avObj->AsTriShape()) {
      visitor(triShape);
    }
  });
}

inline void VisitShapes(
    const RE::NiPointer<RE::NiAVObject>& obj,
    const std::function<void(RE::BSTriShape*)>& visitor) {
  VisitShapes(obj.get(), visitor);
}

inline RE::NiPointer<RE::NiAVObject> GetMaterialSwappableShape(
    RE::NiPointer<RE::NiAVObject> obj) {
  if (!obj) {
    return nullptr;
  }
  if (IsMaterialSwappable(obj)) {
    return obj;
  }
  const auto node = obj->AsNode();
  if (!node) {
    return nullptr;
  }
  for (auto& child : node->children) {
    if (!child) {
      continue;
    }
    if (IsMaterialSwappable(child)) {
      return child;
    }
    if (auto found = GetMaterialSwappableShape(child)) {
      return found;
    }
  }
  return nullptr;
}

inline std::vector<RE::BSTriShape*> GetAllTriShapes(RE::NiAVObject* obj) {
  std::vector<RE::BSTriShape*> shapes;
  if (!obj) {
    return shapes;
  }
  VisitNiObject(obj, [&](RE::NiAVObject* avObj) {
    if (const auto triShape = avObj->AsTriShape()) {
      shapes.emplace_back(triShape);
    }
  });
  return shapes;
}

inline std::vector<RE::BSTriShape*> GetAllTriShapes(
    const RE::NiPointer<RE::NiAVObject>& obj) {
  return GetAllTriShapes(obj.get());
}

inline std::vector<RE::BSTriShape*> GetShapesWithDefaultMaterials(
    RE::NiAVObject* obj) {
  std::vector<RE::BSTriShape*> shapes;
  if (!obj) {
    return shapes;
  }
  VisitNiObject(obj, [&](RE::NiAVObject* avObj) {
    if (const auto triShape = avObj->AsTriShape()) {
      if (triShape->HasExtraData("DefaultMaterial")) {
        shapes.emplace_back(triShape);
      }
    }
  });
  return shapes;
}

inline std::vector<RE::BSTriShape*> GetShapesWithDefaultMaterials(
    const RE::NiPointer<RE::NiAVObject>& obj) {
  return GetShapesWithDefaultMaterials(obj.get());
}

inline RE::BSTriShape* GetTriShape(RE::TESObjectREFR* refr,
                                   const char* nodeName) {
  if (!refr) {
    logger::error("GetNode called with null refr");
    return nullptr;
  }
  RE::BSTriShape* geometry = nullptr;
  VisitNiObject(RE::NiPointer(refr->Get3D()), [&](RE::NiAVObject* avObj) {
    if (avObj->name == nodeName) {
      if (const auto geom = avObj->AsTriShape()) {
        geometry = geom;
      }
    }
  });
  return geometry;
}

inline RE::BSLightingShaderProperty* GetShaderProperty(
    RE::BSTriShape* bsTriShape) {
  auto* properties = bsTriShape->properties[RE::BSGeometry::States::kEffect].get();
  auto* bsLightShader =
      properties ? netimmerse_cast<RE::BSLightingShaderProperty*>(properties)
                 : nullptr;
  return bsLightShader;
}

inline RE::NiAlphaProperty* GetAlphaProperty(RE::BSTriShape* bsTriShape) {
  auto* properties = bsTriShape->properties[RE::BSGeometry::States::kProperty].get();
  auto* alphaProperty =
      properties ? netimmerse_cast<RE::NiAlphaProperty*>(properties) : nullptr;
  return alphaProperty;
}

}  // namespace NifHelpers