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

static bool ApplySavedMaterial_Impl(ArmorFactory* factory, RE::Actor* refr,
                                    const Helpers::InventoryItem* equippedItem,
                                    bool firstLoad) {
  RETURN_IF_FALSE(refr)
  auto* actor = refr->As<RE::Actor>();
  RETURN_IF_FALSE(actor)
  auto* armo = equippedItem->data->object->As<RE::TESObjectARMO>();
  if (!armo) {
    _WARN("Equipped item is not ARMO");
    return false;
  }
  if (equippedItem->uid == NULL) {
    _WARN("Equipped item has no unique ID");
    return true;
  }
  factory->VisitAppliedMaterials(
      equippedItem->object->GetFormID(), equippedItem->uid,
      [&](const char*, const MaterialConfig& appliedMaterialConfig) {
        if (!factory->ApplyMaterial(refr, armo, &appliedMaterialConfig,
                                    !firstLoad)) {
          _ERROR(
              "Failed to apply material to reference: {}, form: {}, "
              "unique ID: {}",
              refr->GetFormID(), armo->GetFormID(), equippedItem->uid);
        }
        return VisitControl::kContinue;
      });
  return true;
}

static bool ApplyMaterialToRefr(RE::TESObjectREFR* refr,
                                const RE::TESObjectARMO* form, int uid,
                                const MaterialConfig* material) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(form)
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
    if (materialFile->color.has_value()) {
      auto color = materialFile->color.value();
      // convert color to uint32_t
      auto colorInt = (static_cast<uint32_t>(color[3] * 255) << 24) |
                      (static_cast<uint32_t>(color[0] * 255) << 16) |
                      (static_cast<uint32_t>(color[1] * 255) << 8) |
                      (static_cast<uint32_t>(color[2] * 255));
    }
  }
  return true;
}

void ArmorFactory::OnUpdate() {
  if (updateStack_.empty()) {
    return;  // No updates to process
  }
  auto limit = 30;
  auto count = 0;
  while (count < limit && !updateStack_.empty()) {
    count++;
    auto& [refr, armo] = updateStack_.top();
    updateStack_.pop();
    if (!refr) {
      _WARN("Invalid reference handle: 0");
      continue;
    }
    auto actor = refr->As<RE::Actor>();
    if (!actor->Is3DLoaded()) {
      updateStack_.push({.refr = refr, .armo = armo});
      continue;
    }
    if (armo) {
      auto uid = Helpers::GetUniqueID(actor, armo->GetSlotMask(), false);
      if (uid == NULL) {
        continue;
      }
      auto* arma = armo->GetArmorAddon(actor->GetRace());
      if (!arma) {
        _WARN("No armor addon found for armor: {}", armo->GetFormID());
        continue;
      }
      actor->VisitArmorAddon(armo, arma, [&](bool, RE::NiAVObject& obj) {
        auto* shape = obj.AsTriShape();
        auto* material =
            shape ? shape->properties[RE::BSGeometry::States::kEffect].get()
                  : nullptr;
        if (!material) {
          return VisitControl::kContinue;
        }
        if (std::string(material->name).ends_with(".json")) {
          auto* materialFile =
              MaterialLoader::LoadMaterial(material->name.c_str());
          if (!materialFile) {
            _ERROR("Failed to load material file: {}", material->name.c_str());
            return VisitControl::kContinue;
          }
          MaterialHelpers::ApplyMaterialToNode(actor, shape, materialFile);
        }
        return VisitControl::kContinue;
      });
      auto* item = Helpers::GetInventoryItemWithUID(actor, uid);
      if (!item) {
        _WARN("No inventory item found for uniqueID: {}", uid);
        continue;
      }
      ApplySavedMaterial_Impl(this, actor, item, false);
    } else {
      RE::BSVisit::TraverseScenegraphObjects(
          actor->Get3D(), [&](RE::NiAVObject* geometry) {
            auto* triShape = geometry->AsTriShape();
            if (!triShape) {
              return VisitControl::kContinue;
            }
            auto& material =
                triShape->properties[RE::BSGeometry::States::kEffect];
            if (!material) {
              return VisitControl::kContinue;
            }
            if (std::string(material->name).ends_with(".json")) {
              auto materialFile =
                  MaterialLoader::LoadMaterial(material->name.c_str());
              if (!materialFile) {
                _ERROR("Failed to load material file: {}",
                       material->name.c_str());
                return VisitControl::kContinue;
              }
              MaterialHelpers::ApplyMaterialToNode(actor, triShape,
                                                   materialFile);
            }
            return VisitControl::kContinue;
          });
    }

    Helpers::VisitEquippedInventoryItems(
        actor, [&](const Helpers::InventoryItem* item) {
          if (item->uid != NULL) {
            ApplySavedMaterial_Impl(this, actor->As<RE::Actor>(), item, true);
          }
          return VisitControl::kContinue;
        });
  }
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
  if (!ApplyMaterialToRefr(refr, form, uid, material)) {
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

bool ArmorFactory::ApplySavedMaterials(RE::TESObjectREFR* refr) {
  RETURN_IF_FALSE(refr)
  updateStack_.push({.refr = refr});
  return true;
}

bool ArmorFactory::ApplySavedMaterials(RE::TESObjectREFR* refr,
                                       RE::TESObjectARMO* armo) {
  RETURN_IF_FALSE(refr)
  RETURN_IF_FALSE(armo)
  if (!MaterialLoader::HasMaterialConfigs(armo->GetFormID())) {
    return false;
  }
  updateStack_.push(UpdateRequest{.refr = refr, .armo = armo});
  return true;
}

void ArmorFactory::ReadFromSave(SKSE::SerializationInterface* iface,
                                Save::SaveData& saveData) {
  stack<UpdateRequest>().swap(updateStack_);
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
  updateStack_.push({.refr = refr});
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