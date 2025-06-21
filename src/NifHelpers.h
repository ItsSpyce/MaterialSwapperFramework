#pragma once
#include "StringHelpers.h"

namespace NifHelpers {
template <typename T>
static bool IsMaterialPath(T path) {
  auto str = StringHelpers::ToLower(path);
  return str.ends_with(".bgsm") || str.ends_with(".bgem");
}

static bool HasBuiltInMaterial(RE::NiObjectNET* obj) {
  if (!obj) {
    return false;
  }
  if (IsMaterialPath(obj->name)) {
    return true;
  }
  return false;
}

static bool HasBuiltInMaterial(const RE::NiPointer<RE::NiObjectNET>& obj) {
  return HasBuiltInMaterial(obj.get());
}

static bool IsMaterialSwappable(RE::NiObjectNET* obj) {
  if (!obj) {
    return false;
  }
  if (obj->HasExtraData("DefaultMaterial")) {
    return true;
  }
  return HasBuiltInMaterial(obj);
}

static bool IsMaterialSwappable(const RE::NiPointer<RE::NiObjectNET>& obj) {
  return IsMaterialSwappable(obj.get());
}

static void VisitNiObject_Impl(
    RE::NiAVObject* obj, const std::function<void(RE::NiAVObject*)>& visitor,
    std::vector<RE::BSFixedString>& visited) {
  if (!obj) {
    return;
  }
  visitor(obj);
  if (const auto node = obj->AsNode()) {
    for (auto& child : node->children) {
      if (!child) {
        continue;
      }
      // Avoid visiting the same object multiple times
      if (!child->name.empty() &&
          std::ranges::find(visited, child->name) != visited.end()) {
        continue;
      }
      visited.emplace_back(child->name);
      VisitNiObject_Impl(child.get(), visitor, visited);
    }
  }
}

static void VisitNiObject(RE::NiAVObject* obj,
                          const std::function<void(RE::NiAVObject*)>& visitor) {
  std::vector<RE::BSFixedString> visited{};
  VisitNiObject_Impl(obj, visitor, visited);
}

static void VisitNiObject(const RE::NiPointer<RE::NiAVObject>& obj,
                          const std::function<void(RE::NiAVObject*)>& visitor) {
  VisitNiObject(obj.get(), visitor);
}

static RE::NiPointer<RE::NiAVObject> GetMaterialSwappableShape(
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

static std::vector<RE::BSTriShape*> GetAllTriShapes(RE::NiAVObject* obj) {
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

static std::vector<RE::BSTriShape*> GetAllTriShapes(
    const RE::NiPointer<RE::NiAVObject>& obj) {
  return GetAllTriShapes(obj.get());
}

static std::vector<RE::BSTriShape*> GetShapesWithDefaultMaterials(
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

static std::vector<RE::BSTriShape*> GetShapesWithDefaultMaterials(
    const RE::NiPointer<RE::NiAVObject>& obj) {
  return GetShapesWithDefaultMaterials(obj.get());
}

static RE::BSTriShape* GetTriShape(RE::TESObjectREFR* refr,
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

static RE::BSLightingShaderProperty* GetShaderProperty(RE::BSTriShape* shape) {
  if (!shape) {
    logger::error("GetShaderProperty called with null shape");
    return nullptr;
  }
  for (auto& property : shape->properties) {
    if (const auto shaderProperty =
            skyrim_cast<RE::BSLightingShaderProperty*>(property.get())) {
      return shaderProperty;
    }
  }

  return nullptr;
}
}  // namespace NifHelpers