#include "ArmorFactory.h"

#include <DirectXTex.h>

#include <srell.hpp>

#include "Helpers.h"
#include "IO/MaterialLoader.h"
#include "MaterialHelpers.h"
#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"
#include "Save/Types.h"

using ArmorFactory = Factories::ArmorFactory;
using ShaderFlag = RE::BSShaderProperty::EShaderPropertyFlag;
using Texture = RE::BSShaderTextureSet::Texture;
using VisitControl = RE::BSVisit::BSVisitControl;

static void ClearItemDisplayName(const RE::InventoryEntryData* data) {
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

static void SetItemDisplayName(ArmorFactory* factory, RE::TESObjectREFR* refr,
                               UniqueID uid) {
  auto* item = Helpers::GetInventoryItemWithUID(refr, uid);
  const auto data = item ? item->data.get() : nullptr;
  if (!data) {
    return;
  }
  vector<const char*> filteredMaterials;
  factory->VisitAppliedMaterials(
      item->object->GetFormID(), uid,
      [&](const char* name, const MaterialConfig& config) {
        if (!config.isHidden && config.modifyName) {
          filteredMaterials.emplace_back(name);
          _TRACE("Adding material to display name: {}", name);
        }
        return VisitControl::kContinue;
      });

  if (filteredMaterials.empty()) {
    ClearItemDisplayName(data);
    return;
  }
  const auto name = fmt::format("{} [{}]", item->object->GetName(),
                                fmt::join(filteredMaterials, ", "));
  auto* front = Helpers::GetOrCreateExtraList(item->data.get());
  if (!front) {
    return;
  }
  if (auto* textDisplayData = front->GetByType<RE::ExtraTextDisplayData>()) {
    textDisplayData->SetName(name.c_str());
  } else {
    textDisplayData = new RE::ExtraTextDisplayData(name.c_str());
    front->Add(textDisplayData);
  }
}

static bool ApplyMaterialToRefr(RE::TESObjectREFR* refr,
                                const MaterialConfig* material) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(material)
  auto* refrModel = refr->Get3D();
  for (const auto& [shapeName, materialName] : material->applies) {
    auto* niAv = refrModel->GetObjectByName(shapeName);
    if (!niAv) {
      _WARN("No object found for shape name: {}", shapeName);
      continue;
    }
    auto* triShape = niAv->AsTriShape();
    if (!triShape) {
      _WARN("No tri-shape found for shape name: {}", shapeName);
      continue;
    }
    auto* materialFile = MaterialLoader::LoadMaterial(materialName);
    if (!materialFile) {
      _ERROR("Failed to load material file: {}", materialName);
      continue;
    }
    MaterialHelpers::ApplyMaterialToNode(refr, triShape, materialFile);
  }
  return true;
}

bool ArmorFactory::ApplyMaterial(RE::TESObjectREFR* refr,
                                 RE::TESObjectARMO* form,
                                 const MaterialConfig* material,
                                 bool overwriteName) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(form)
  auto uid = Helpers::GetUniqueID(refr, form->GetSlotMask(), true);
  if (uid == NULL) {
    _WARN("Failed to get unique ID for form: {}", form->GetFormID());
    return false;
  }
  _TRACE("Applying material {} to UID {}", material->name, uid);
  if (!ApplyMaterialToRefr(refr, material)) {
    _ERROR("Failed to apply material to reference: {}, form: {}, unique ID: {}",
           refr->GetFormID(), form->GetFormID(), uid);
    return false;
  }
  armorData_.try_emplace(uid, ArmorData{});
  auto& appliedMaterials = armorData_[uid];
  if (std::ranges::contains(appliedMaterials.materials, material->name)) {
    return true;  // Material already applied
  }
  vector<string> newAppliedMaterials;
  for (const auto& mat : appliedMaterials.materials) {
    const auto* matConfig =
        MaterialLoader::GetMaterialConfig(form->GetFormID(), mat);
    if (matConfig && matConfig->layer != material->layer) {
      newAppliedMaterials.push_back(mat);
    }
  }

  newAppliedMaterials.push_back(material->name);
  appliedMaterials.materials = newAppliedMaterials;
  if (overwriteName) {
    SetItemDisplayName(this, refr, uid);
  }

  return true;
}

bool ArmorFactory::ApplySavedMaterials(RE::Actor* actor, RE::NiNode* armor,
                                       RE::NiAVObject*, i32 bipedSlot) {
  RETURN_IF_FALSE(actor)
  if (!actor->Is3DLoaded()) {
    return false;
  }
  auto* armorInSlot = actor->GetWornArmor(
      static_cast<RE::BGSBipedObjectForm::BipedObjectSlot>(1 << bipedSlot));
  auto uid = armorInSlot ? Helpers::GetUniqueID(
                               actor, armorInSlot->GetSlotMask(), false)
                         : 0;
  auto armorData = uid != 0 && !armorData_.contains(uid)
                       ? nullopt
                       : optional(armorData_[uid]);
  RE::BSVisit::TraverseScenegraphObjects(
      actor->Get3D(), [&](RE::NiAVObject* geometry) {
        auto* triShape = geometry->AsTriShape();
        if (!triShape) {
          return VisitControl::kContinue;
        }
        const auto& material =
            triShape->properties[RE::BSGeometry::States::kEffect];
        if (!material) {
          return VisitControl::kContinue;
        }

        if (armorData.has_value() && armorInSlot) {
          _DEBUG("Applying saved materials to armor in slot: {0:X}, UID: {1}",
                 armorInSlot->GetFormID(), uid);
          for (const auto& materialName : armorData->materials) {
            auto* materialConfig = MaterialLoader::GetMaterialConfig(
                armorInSlot->GetFormID(), materialName);
            if (!materialConfig) {
              _ERROR("No material config found for material: {}", materialName);
              continue;
            }
            auto appliesEntry = materialConfig->applies.contains(triShape->name.c_str()) ?
                                    materialConfig->applies.at(triShape->name.c_str()) : "";
            if (appliesEntry.empty()) {
              _DEBUG("No applies entry for shape: {}", triShape->name);
              continue;
            }
            auto* materialFile =
                MaterialLoader::LoadMaterial(appliesEntry);
            if (!materialFile) {
              _ERROR("Failed to load material file: {}", appliesEntry);
              continue;
            }
            MaterialHelpers::ApplyMaterialToNode(actor, triShape, materialFile);
          }
        } else if (std::string(material->name).ends_with(".json")) {
          _DEBUG("Applying default material from file: {}", material->name);
          auto materialFile =
              MaterialLoader::LoadMaterial(material->name.c_str());
          if (!materialFile) {
            _ERROR("Failed to load material file: {}", material->name.c_str());
            return VisitControl::kContinue;
          }
          MaterialHelpers::ApplyMaterialToNode(actor, triShape, materialFile);
        }
        return VisitControl::kContinue;
      });

  return true;
}

void ArmorFactory::ReadFromSave(SKSE::SerializationInterface* iface,
                                Save::SaveData& saveData) {
  for (const auto& saveRecords : saveData.armorRecords | views::values) {
    for (auto& [uid, appliedMaterials] : saveRecords) {
      if (appliedMaterials.empty()) {
        continue;
      }
      vector<string> validMaterials;
      for (const auto& material : appliedMaterials) {
        if (material.empty()) {
          continue;  // Skip empty material names
        }
        validMaterials.push_back(material);
      }
      if (validMaterials.empty()) {
        continue;
      }
      armorData_.emplace(uid,
                         ArmorData{.materials = std::move(validMaterials)});
    }
  }
}

void ArmorFactory::WriteToSave(SKSE::SerializationInterface* iface,
                               Save::SaveData& saveData) {
  saveData.armorRecords.clear();
  for (auto& [uniqueID, records] : armorData_) {
    if (records.materials.empty()) {
      continue;
    }
    const auto formID = Helpers::GetFormIDForUniqueID(uniqueID);
    if (formID == 0) {
      _WARN("Invalid form ID for unique ID: {}", uniqueID);
      continue;
    }
    ArmorRecordEntryV2 record{.uniqueID = uniqueID,
                              .appliedMaterials = records.materials};
    if (saveData.armorRecords.contains(formID)) {
      saveData.armorRecords[formID].push_back(record);
    } else {
      saveData.armorRecords.emplace(formID, vector{record});
    }
  }
}

void ArmorFactory::ResetMaterials(RE::TESObjectREFR* refr) {
  if (!refr) {
    return;
  }
  auto* actor = refr->As<RE::Actor>();
  if (!actor) {
    return;
  }
  Helpers::VisitInventoryItems(actor, [&](const Helpers::InventoryItem* item) {
    if (item->uid == NULL) {
      return VisitControl::kContinue;
    }
    armorData_.erase(item->uid);
    ClearItemDisplayName(item->data.get());
    item->data->extraLists->front()->RemoveByType(RE::ExtraDataType::kUniqueID);
    return VisitControl::kContinue;
  });
}

void ArmorFactory::VisitAppliedMaterials(
    const RE::FormID formID, const UniqueID uid,
    const Visitor<const char*, const MaterialConfig&>& visitor) const {
  if (uid == NULL) {
    return;
  }
  auto* armo = RE::TESForm::LookupByID<RE::TESObjectARMO>(formID);
  if (!armo) {
    _ERROR("Form ID {} is not a valid armor", formID);
    return;
  }
  if (!armorData_.contains(uid)) {
    return;
  }
  const auto& appliedMaterials = armorData_.at(uid);
  for (const auto& materialName : appliedMaterials.materials) {
    if (auto* materialConfig =
            MaterialLoader::GetMaterialConfig(formID, materialName)) {
      if (visitor(materialName.c_str(), *materialConfig) ==
          VisitControl::kStop) {
        return;
      }
    }
  }
}