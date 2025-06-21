#include "MaterialWriter.h"

#include "NiOverride.h"
#include "NifHelpers.h"

static void SetItemDisplayName(
    const std::unique_ptr<RE::InventoryEntryData>& data,
    const std::string& name) {
  if (!data) {
    return;
  }
  if (data->extraLists) {
    for (auto& extraList : *data->extraLists) {
      if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
        auto textDisplayData = extraList->GetByType<RE::ExtraTextDisplayData>();
        textDisplayData->SetName(name.c_str());
        return;
      } else {
        auto textDisplayData = new RE::ExtraTextDisplayData(name.c_str());
        extraList->Add(textDisplayData);
        return;
      }
    }
  }
}

static void ClearItemDisplayName(
    const std::unique_ptr<RE::InventoryEntryData>& data) {
  if (!data) {
    return;
  }
  if (data->extraLists) {
    for (auto& extraList : *data->extraLists) {
      if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
        extraList->RemoveByType(RE::ExtraDataType::kTextDisplayData);
        return;
      }
    }
  }
}

static std::string AppendMaterialName(RE::TESForm* form,
                                      const MaterialRecord& material) {
  std::string name(form->GetName());
  std::string nameWithoutPreviousMaterial;
  if (size_t index; StringHelpers::HasMaterialName(form, index)) {
    nameWithoutPreviousMaterial = name.substr(0, index);
  } else {
    nameWithoutPreviousMaterial = name;
  }
  return fmt::format("{} (*{})", nameWithoutPreviousMaterial, material.name);
}

bool MaterialWriter::ApplyMaterial(RE::TESObjectREFR* refr,
                                   RE::BIPED_OBJECTS::BIPED_OBJECT armorSlot,
                                   const MaterialRecord& material) {
  RETURN_IF_FALSE(refr)
  auto materialFile = MaterialLoader::LoadMaterial(material);
  RETURN_IF_FALSE(materialFile)
  logger::debug("Material info: name ({}), filename ({})", material.name,
                material.filename);
  auto& bipedObject = refr->GetCurrentBiped()->objects[armorSlot];
  RETURN_IF_FALSE(bipedObject.item)
  auto armo = bipedObject.item->As<RE::TESObjectARMO>();
  auto inventoryItem =
      Helpers::GetInventoryItemWithFormID(refr, armo->GetFormID());
  RETURN_IF_FALSE(inventoryItem.data)
  if (material.isDefault) {
    ClearItemDisplayName(inventoryItem.data);
  } else {
    const auto newDisplayName = AppendMaterialName(armo, material);
    SetItemDisplayName(inventoryItem.data, newDisplayName);
  }
  if (inventoryItem.data->extraLists) {
    if (auto actor = refr->As<RE::Actor>()) {
      auto actor3d = actor->Get3D();
      auto shapes = NifHelpers::GetAllTriShapes(actor3d);
      for (auto& shape : shapes) {
        if (std::ranges::find(material.shapes, std::string{shape->name}) !=
            material.shapes.end()) {
          logger::debug("Applying material to tri shape: {}", shape->name);
          NiOverride::ApplyMaterialToNode(refr, true, shape->name.c_str(),
                                          *materialFile);
        }
      }
    }
  }
  return false;
}

bool MaterialWriter::ApplyBuiltInMaterials(RE::TESObjectREFR* refr) {
  RETURN_IF_FALSE(refr)
  auto triShapes = NifHelpers::GetAllTriShapes(refr->Get3D());
  for (auto& triShape : triShapes) {
    auto material = NifHelpers::GetShaderProperty(triShape);
    if (!material) {
      continue;
    }
    if (NifHelpers::HasBuiltInMaterial(material)) {
      logger::debug("Applying default material to tri shape: {}",
                    triShape->name);
      auto materialFile = MaterialLoader::LoadMaterial(material->name.c_str());
      if (!materialFile) {
        logger::error("Failed to load material file: {}",
                      material->name.c_str());
        continue;
      }
      NiOverride::ApplyMaterialToNode(refr, true, triShape->name.c_str(),
                                      *materialFile);
    }
  }
  return true;
}

bool MaterialWriter::ApplySavedMaterials(RE::TESObjectREFR* refr) {
  RETURN_IF_FALSE(refr)
  auto equippedItems = Helpers::GetEquippedInventoryItems(refr);
  if (equippedItems.empty()) {
    logger::warn("No equipped items found for reference: {}",
                 refr->GetFormID());
    return false;
  }
  std::vector<MaterialRecord> materials{};
  for (const auto& inventoryItem : equippedItems) {
    auto formID = inventoryItem.object->GetFormID();
    auto name = inventoryItem.data->GetDisplayName();
    logger::debug("Processing item: {}, form ID: {}", name, formID);
    if (size_t matIndex = 0; StringHelpers::HasMaterialName(name, matIndex)) {
      auto materialName = std::string(name).substr(
          matIndex + 3, std::strlen(name) - (matIndex + 3) - 1);
      auto& materialRecord = MaterialLoader::GetMaterial(formID, materialName);
      materials.emplace_back(materialRecord);
    }
  }
  auto triShapes = NifHelpers::GetAllTriShapes(refr->Get3D());
  for (auto& triShape : triShapes) {
    auto material = NifHelpers::GetShaderProperty(triShape);
    if (!material) {
      continue;
    }
    auto nodeName = triShape->name.c_str();
    auto it = std::ranges::find_if(materials, [&](const MaterialRecord& mat) {
      return std::ranges::find(mat.shapes, nodeName) != mat.shapes.end();
    });
    if (it == materials.end()) {
      if (NifHelpers::HasBuiltInMaterial(material)) {
        logger::debug("Applying default material to tri shape: {}",
                      triShape->name);
        auto materialFile =
            MaterialLoader::LoadMaterial(material->name.c_str());
        if (!materialFile) {
          logger::error("Failed to load material file: {}",
                        material->name.c_str());
          continue;
        }
        NiOverride::ApplyMaterialToNode(refr, true, nodeName, *materialFile);
      }
    } else {
      auto materialFile = MaterialLoader::LoadMaterial(*it);
      if (!materialFile) {
        logger::error("Failed to load material file: {}", it->filename);
        continue;
      }
      NiOverride::ApplyMaterialToNode(refr, true, nodeName, *materialFile);
    }
  }
  return true;
}