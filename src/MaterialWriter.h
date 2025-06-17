#pragma once

#include "NifHelpers.h"
#include "NiOverride.h"
#include "MaterialLoader.h"
#include "MaterialCache.h"

namespace MaterialWriter {
inline bool ApplyMaterial(RE::Actor* actor,
                   RE::BIPED_OBJECTS::BIPED_OBJECT armorSlot,
                   const std::string& material) {
  RETURN_IF_FALSE(actor)
  auto materialFile = MaterialLoader::LoadMaterial(material);
  RETURN_IF_FALSE(materialFile)
  return false;
}
inline bool ApplyMaterial(const RE::Actor* actor,
                   RE::BIPED_OBJECTS::BIPED_OBJECT armorSlot,
                   const std::string& shapeName, const std::string& material) {
  return false;
}
inline bool ApplyDefaultMaterial(RE::Actor* actor,
                          RE::BIPED_OBJECTS::BIPED_OBJECT armorSlot) {
  logger::info("Applying default materials");
  RETURN_IF_FALSE(actor)
  const auto& biped = actor->GetCurrentBiped()->objects[armorSlot];
  RETURN_IF_FALSE(biped.item)
  if (auto armor = biped.item->As<RE::TESObjectARMO>(); armor->IsInventoryObject()) {
    
  }

  auto shapesWithMaterials =
      NifHelpers::GetShapesWithDefaultMaterials(biped.partClone);
  for (auto& shape : shapesWithMaterials) {
    auto materialName =
        shape->GetExtraData<RE::NiStringExtraData>("DefaultMaterial")->value;
    // Apply the default material
    if (std::strlen(materialName) != 0) {
      if (!ApplyMaterial(actor, armorSlot, shape->name.c_str(), materialName)) {
        logger::error("Failed to apply material {} to shape: {}", materialName,
                      shape->name);
      }
    }
  }

  return true;
}
}  // namespace MaterialWriter