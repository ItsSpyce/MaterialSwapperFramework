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

inline std::vector<RE::BSGeometry*> GetAllTriShapes(RE::NiAVObject* obj) {
  std::vector<RE::BSGeometry*> shapes;
  if (!obj) {
    return shapes;
  }
  RE::BSVisit::TraverseScenegraphGeometries(obj, [&](RE::BSGeometry* geometry) {
    shapes.emplace_back(geometry);
    return RE::BSVisit::BSVisitControl::kContinue;
  });
  return shapes;
}

inline std::vector<RE::BSGeometry*> GetAllTriShapes(
    const RE::NiPointer<RE::NiAVObject>& obj) {
  return GetAllTriShapes(obj.get());
}

inline std::vector<RE::BSGeometry*> GetShapesWithDefaultMaterials(
    RE::NiAVObject* obj) {
  std::vector<RE::BSGeometry*> shapes;
  if (!obj) {
    return shapes;
  }
  RE::BSVisit::TraverseScenegraphGeometries(obj, [&](RE::BSGeometry* avObj) {
    if (const auto triShape = avObj->AsTriShape()) {
      if (triShape->HasExtraData("DefaultMaterial")) {
        shapes.emplace_back(triShape);
      }
    }
    return RE::BSVisit::BSVisitControl::kContinue;
  });
  return shapes;
}

inline std::vector<RE::BSGeometry*> GetShapesWithDefaultMaterials(
    const RE::NiPointer<RE::NiAVObject>& obj) {
  return GetShapesWithDefaultMaterials(obj.get());
}

inline RE::BSGeometry* GetTriShape(RE::TESObjectREFR* refr,
                                   const char* nodeName) {
  if (!refr) {
    logger::error("GetNode called with null refrHandle");
    return nullptr;
  }
  auto* nif = refr->Get3D();
  if (!nif) {
    return nullptr;
  }
  return nif->GetObjectByName(nodeName)->AsGeometry();
}

inline RE::BSLightingShaderProperty* GetShaderProperty(
    RE::BSGeometry* bsTriShape) {
  auto* properties =
      bsTriShape->properties[RE::BSGeometry::States::kEffect].get();
  auto* bsLightShader =
      properties ? netimmerse_cast<RE::BSLightingShaderProperty*>(properties)
                 : nullptr;
  return bsLightShader;
}

inline RE::NiAlphaProperty* GetAlphaProperty(RE::BSGeometry* bsTriShape) {
  auto* properties =
      bsTriShape->properties[RE::BSGeometry::States::kProperty].get();
  auto* alphaProperty =
      properties ? netimmerse_cast<RE::NiAlphaProperty*>(properties) : nullptr;
  return alphaProperty;
}

}  // namespace NifHelpers