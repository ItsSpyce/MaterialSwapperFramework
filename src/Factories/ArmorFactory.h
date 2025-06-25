#pragma once

#include "Factory.h"
#include "NiOverride.h"
#include "NifHelpers.h"
#include "Singleton.h"

namespace Factories {
class ArmorFactory : public Factory<RE::TESObjectARMO>,
                     public Singleton<ArmorFactory> {
 public:
  bool ApplyMaterial(RE::TESObjectREFR* refr, RE::TESObjectARMO* form,
                     const MaterialRecord& material) override {
    RETURN_IF_FALSE(refr)
    auto materialFile = MaterialLoader::LoadMaterial(material);
    RETURN_IF_FALSE(materialFile)
    logger::debug("Material info: name ({}), filename ({})", material.name,
                  material.filename);
    auto& bipedObject =
        refr->GetCurrentBiped()->objects[ConvertSlotMask(form->GetSlotMask())];
    RETURN_IF_FALSE(bipedObject.item)
    auto armo = bipedObject.item->As<RE::TESObjectARMO>();
    auto inventoryItem =
        Helpers::GetInventoryItemWithFormID(refr, armo->GetFormID());
    RETURN_IF_FALSE(inventoryItem.data)
    if (!material.modifyName) {
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

  bool ApplyDefaultMaterial(RE::TESObjectREFR* refr,
                            RE::TESObjectARMO*) override {
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
        auto materialFile =
            MaterialLoader::LoadMaterial(material->name.c_str());
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

  bool ApplySavedMaterial(RE::TESObjectREFR* refr,
                          RE::TESObjectARMO*) override {
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
        if (auto materialRecord = MaterialLoader::GetMaterial(formID, materialName)) {
          logger::debug("Found material record: {}", materialRecord->name);
          materials.push_back(*materialRecord);
        } else {
          logger::warn("No material record found for form ID: {}, name: {}",
                       formID, materialName);
        }
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

 private:
  static constexpr RE::BIPED_OBJECTS::BIPED_OBJECT ConvertSlotMask(
      const RE::BGSBipedObjectForm::BipedObjectSlot slot) {
    // this is a lot easier to do in the inverse direction...
    switch (slot) {
      case RE::BGSBipedObjectForm::BipedObjectSlot::kHead:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kHead;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kHair:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kHair;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kBody:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kBody;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kHands:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kHands;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kForearms:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kForearms;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kAmulet:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kAmulet;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kRing:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kRing;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kFeet:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kFeet;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kCalves:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kCalves;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kShield:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kShield;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kTail:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kTail;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kLongHair;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kCirclet;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kEars:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kEars;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModMouth:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModMouth;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModNeck:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModNeck;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModChestPrimary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModChestPrimary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModBack:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModBack;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc1:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModMisc1;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisPrimary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModPelvisPrimary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitateHead:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kDecapitateHead;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitate:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kDecapitate;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisSecondary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModPelvisSecondary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModLegRight:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModLegRight;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModLegLeft:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModLegLeft;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModFaceJewelry:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModFaceJewelry;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModChestSecondary:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModChestSecondary;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModShoulder:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModShoulder;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModArmLeft:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModArmLeft;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModArmRight:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModArmRight;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc2:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kModMisc2;
      case RE::BGSBipedObjectForm::BipedObjectSlot::kFX01:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kFX01;
      default:
        return RE::BIPED_OBJECTS::BIPED_OBJECT::kNone;
    }
  }

  static void SetItemDisplayName(
      const std::unique_ptr<RE::InventoryEntryData>& data,
      const std::string& name) {
    if (!data) {
      return;
    }
    if (data->extraLists) {
      for (auto& extraList : *data->extraLists) {
        if (extraList->HasType(RE::ExtraDataType::kTextDisplayData)) {
          auto textDisplayData =
              extraList->GetByType<RE::ExtraTextDisplayData>();
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
};
}  // namespace Factories