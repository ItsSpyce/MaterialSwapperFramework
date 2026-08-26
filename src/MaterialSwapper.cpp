#include "MaterialSwapper.h"

#include <emhash/hash_table5.hpp>

#include "Cache/FilenameIDCache.h"
#include "Conditions/ConditionParser.h"
#include "Filesystem.h"
#include "Helpers/JsonHelpers.h"
#include "Helpers/RaceMenuHelpers.h"
#include "Helpers/SkyrimHelpers.h"
#include "MeshBuilder.h"
#include "ModState.h"

#undef GetObject

namespace {
namespace fs = std::filesystem;
using RE::BSVisit::BSVisitControl;

std::mutex g_lock;
emhash5::HashMap<RE::FormID, emhash8::HashMap<std::string, MATC>> g_configs;
emhash5::HashMap<FileID, MATR> g_records;
constexpr auto DEFAULT_MATERIAL_KEY = "__DEFAULT__";


result<MATR> ReadMaterialJson(const std::string& filename) noexcept {
  glz::generic json;
  MATR record;
  if (!fs::exists(filename)) {
    return Err{"Material file does not exist: {}", filename};
  }
  record.id = FilenameIDCache::GetFilenameID(filename);
  if (record.id == NULL) return Err{"Failed to reserve material cache file"};
  if (const auto it = g_records.find(record.id); it != g_records.end()) {
    return Ok{it->second};
  }
  if (auto err = glz::read_file_jsonc(json, filename, std::string{})) {
    auto cleanedErr = glz::format_error(err);
    return Err{"Failed to read material file {}: {}", filename, cleanedErr};
  }
  if (auto inherits = JsonHelpers::MaybeGet<std::string>(json, "inherits")) {
    if (auto parent = ReadMaterialJson(inherits.value())) {
      record.inherits = parent.value().id;
    }
  }
  {
#define SET_TEXTURE(_TEXTURE)                       \
  record._TEXTURE = FilenameIDCache::GetFilenameID( \
      JsonHelpers::MaybeGet<std::string>(json, #_TEXTURE).value_or(""));
    SET_TEXTURE(diffuseMap);
    SET_TEXTURE(normalMap);
    SET_TEXTURE(smoothSpecMap);
    SET_TEXTURE(grayscaleMap);
    SET_TEXTURE(glowMap);
    SET_TEXTURE(wrinkleMap);
    SET_TEXTURE(specularMap);
    SET_TEXTURE(lightingMap);
    SET_TEXTURE(flowMap);
    SET_TEXTURE(distanceFieldAlphaMap);
    SET_TEXTURE(envMap);
    SET_TEXTURE(innerLayerMap);
    SET_TEXTURE(displacementMap);
    SET_TEXTURE(baseMap);
    SET_TEXTURE(envMapMask);
    SET_TEXTURE(faceTintMap);
    SET_TEXTURE(detailMap);
    SET_TEXTURE(subsurfaceMap);
    SET_TEXTURE(parallaxMap);
    SET_TEXTURE(rmaosMap);
    SET_TEXTURE(coatMap);
    SET_TEXTURE(coatNormalMap);
    SET_TEXTURE(fuzzMap);
    SET_TEXTURE(emissionMap);
    SET_TEXTURE(colorBlendMap);
#undef SET_TEXTURE
  }
  // TODO:
  return Ok{record};
}

result<std::vector<MATC>> ReadConfigJson(const std::string& filename) {
  std::vector<MATC> matConfigs;
  if (filename[0] == '_') return Ok{matConfigs};
  std::unordered_map<std::string, std::vector<JsonMATCRecord>> json;
  _INFO("Reading material config {}", filename);
  if (auto err = glz::read_file_json<glz::opts{.error_on_unknown_keys = false,
                                               .error_on_missing_keys = false}>(
          json, filename, std::string{})) {
    return Err{"Failed to read material config file at {}:\r\n{}", filename,
               glz::format_error(err)};
  }
  for (const auto& [formID, configs] : json) {
    for (const auto& config : configs) {
      MATC item;
      item.form = Helpers::GetFormID(formID);
      if (item.form == 0) {
        _ERROR("Unknown form ID {}", item.form);
        continue;
      }
      item.name = config.name;
      item.gender = config.gender;
      item.isHidden = config.isHidden;
      item.layer = config.layer;
      _DEBUG("Parsing MATC {} for form ID {}", item.name, formID);
      for (const auto& condition : config.conditions) {
        MaterialCondition materialCondition;
        if (condition.type == "dayOfWeek") {
          materialCondition.function = MaterialFunctionID::GetDayOfWeek;
        } else if (condition.type == "weather") {
          materialCondition.function = MaterialFunctionID::GetIsCurrentWeather;
        }
        materialCondition.param = condition.value;
        materialCondition.op = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
      }
      for (const auto& filter : config.filters) {
        auto cond = Conditions::ParseFromString(filter.c_str());
        if (cond.is_ok()) {
          item.conditions.emplace_back(cond.value());
        } else {
          _ERROR("Failed to parse condition string for {} {}: {}", item.form,
                 item.name, cond.error());
        }
      }
      for (const auto& [shape, matrFile] : config.applies) {
        const auto path = std::string{"Data/"} +
                          StringHelpers::AssertPrefix(matrFile, "materials/");
        if (const auto matr = ReadMaterialJson(path)) {
          const auto filenameID = FilenameIDCache::GetFilenameID(path);
          g_records[filenameID] = matr.value();
          item.applies.emplace_back(std::tuple(shape, filenameID));
        } else {
          _ERROR("Failed to read material JSON: {}", matr.error());
        }
      }
      matConfigs.emplace_back(item);
    }
  }
  return Ok{matConfigs};
}

RE::NiNode* CloneWithMaterial(RE::NiNode* node, RE::FormID formId,
                              const std::vector<std::string>& materials) {
  RE::BSVisit::TraverseScenegraphObjects(node, [&](RE::NiAVObject* geometry) {
    auto* ts = geometry->AsTriShape();
    if (!ts) return BSVisitControl::kContinue;
    const auto& material = ts->GetGeometryRuntimeData().shaderProperty;
    if (!material) {
      _ERROR("No shader property found");
      return BSVisitControl::kContinue;
    }
    if (material && material->name.contains(".json")) {
      const auto materialFileID =
          FilenameIDCache::GetFilenameID(material->name.c_str());
      if (const auto matr =
              MaterialSwapper::GetMaterialRecord(materialFileID)) {
        MeshBuilder::ApplyMaterialToNode(ts, matr.value());
      } else {
        _ERROR("Failed to load material record {}", material->name.c_str());
        return BSVisitControl::kContinue;
      }
    }
    for (const auto& materialName : materials) {
      auto matc = MaterialSwapper::GetMaterialConfig(formId, materialName);
      if (!matc) {
        _ERROR("No material config found for material {}", materialName);
        continue;
      }
      if (auto materialPath = matc->find_material(ts->name.c_str())) {
        const auto matr =
            MaterialSwapper::GetMaterialRecord(materialPath.value());
        if (!matr) {
          _ERROR("Failed to load material record {}", materialPath.value());
          continue;
        }
        _TRACE("Applying saved material {}", materialName);
        MeshBuilder::ApplyMaterialToNode(ts, matr.value());
      }
    }
    return BSVisitControl::kContinue;
  });
  return node;
}

void UpdateInventoryItemMaterials(const UniqueID uid,
                                  RE::InventoryEntryData* data,
                                  const std::vector<MATC>& materials) {
  ModState::GetSingleton()->SetMaterials(
      uid, materials | std::views::transform([](auto& matc) {
             return matc.name;
           }) | std::ranges::to<std::vector>());
  Helpers::ResetDisplayName(data);

  const auto name = fmt::format(
      "{} [{}]", data->GetDisplayName(),
      StringHelpers::Join(materials | std::views::filter([](auto& matc) {
                            return matc.modifyName;
                          }) | std::views::transform([](auto& matc) {
                            return matc.name;
                          }) | std::ranges::to<std::vector>(),
                          ", "));
  Helpers::SetItemDisplayName(data, name.c_str());
}

void VisitMaterials(const UniqueID uid, const Visitor<MATC>& visitor) {
  auto formID = Helpers::GetFormIDForUniqueID(uid);
  FIND_IN(g_configs, it, formID) {
    for (const auto& materials = ModState::GetSingleton()->GetMaterials(uid);
         const auto& material : materials) {
      FIND_IN(it->second, matcIt, material) {
        if (visitor(matcIt->second) == BSVisitControl::kStop) {
          return;
        }
      }
    }
  }
}
}  // namespace

namespace MaterialSwapper {
void ReadMaterialConfigurations() {
  SCOPE_GUARD(g_lock);
  g_configs.clear();
  // this is faster than for-each over everything apparently?
  const auto globalRdi = Filesystem::EnumerateMaterialConfigDir();
  FOR_IN_DIR(globalRdi, fileIt) {
    const auto configs = ReadConfigJson(fileIt->path().string());
    if (!configs) {
      _ERROR("Failed to read config file {}: {}", fileIt->path().string(),
             configs.error());
      continue;
    }
    for (const auto& config : configs.value()) {
      FIND_IN(g_configs, it, config.form) { it->second[config.name] = config; }
      else {
        g_configs[config.form] = {{config.name, config}};
      }
    }
  }
  for (const auto& modDirectory : Filesystem::EnumerateModsInMaterialDir()) {
    const auto modName = modDirectory.path().filename().string();
    if (!Helpers::IsModName(modName)) continue;
    auto plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(modName);
    if (!plugin) {
      _WARN("Plugin not found: {}", modName);
      continue;
    }
    const auto modRdi = fs::directory_iterator(modDirectory);
    for (auto it = fs::begin(modRdi); it != fs::end(modRdi); ++it) {
      if (!it->is_regular_file() || it->path().extension() != ".json") continue;
      const auto configs = ReadConfigJson(it->path().string());
      if (configs.is_err()) {
        _ERROR("Failed to read material config at {}: {}", it->path().string(),
               configs.error());
        continue;
      }
      for (const auto& config : configs.value()) {
        FIND_IN(g_configs, it, config.form) {
          it->second[config.name] = config;
        }
        else {
          g_configs[config.form] = {{config.name, config}};
        }
      }
    }
  }
}

void VisitMaterialFiles(const RE::FormID formID,
                        const Visitor<const MATC&>& visitor) {
  if (formID == NULL) return;
  FIND_IN(g_configs, it, formID) {
    for (const auto& config : it->second | std::views::values) {
      if (const auto control = visitor(config);
          control == BSVisitControl::kStop)
        return;
    }
  }
}

_NODISCARD std::optional<MATR> GetMaterialRecord(const FileID fileID) {
  if (fileID == NULL) return std::nullopt;
  FIND_IN(g_records, it, fileID) { return it->second; }
  return std::nullopt;
}

_NODISCARD std::optional<MATC> GetMaterialConfig(RE::FormID formID,
                                                 const std::string& name) {
  if (formID == NULL) return std::nullopt;
  FIND_IN(g_configs, it, formID) {
    FIND_IN(it->second, mIt, name) { return mIt->second; }
  }
  return std::nullopt;
}

_NODISCARD std::optional<MATC> GetDefaultMaterialConfig(
    const RE::FormID formID) {
  return GetMaterialConfig(formID, DEFAULT_MATERIAL_KEY);
}

_NODISCARD bool HasMaterialConfigs(const RE::FormID formID) {
  if (formID == NULL) return false;
  return g_configs.contains(formID);
}

void ResetAll() { SCOPE_GUARD(g_lock); }

void VisitApplicableMaterials(const RE::TESForm* form,
                              const Visitor<const MATC&>& visitor) {
  FIND_IN(g_configs, it, form->GetFormID()) {
    for (const auto& matc : it->second | std::views::values) {
      if (visitor(matc) == BSVisitControl::kStop) {
        return;
      }
    }
  }
}

void ResetEquippedArmors(RE::Actor* actor) {
  for (unsigned i = 1; i < 32; ++i) {
    ResetEquippedArmor(actor, (RE::BipedObjectSlot)(1 << i));
  }
}

void ResetEquippedArmor(RE::Actor* actor, const RE::BipedObjectSlot slot) {
  auto* equipped = actor->GetWornArmor(slot);
  auto uid = Helpers::GetUniqueID(actor, slot, false);
  if (!equipped || uid == NULL) return;
  SCOPE_GUARD(g_lock);
}

void ApplyArmorMaterial(RE::Actor* actor, const RE::BipedObjectSlot slot,
                        const MATC& config) {
  auto* equipped = actor->GetWornArmor(slot);
  auto uid = Helpers::GetUniqueID(actor, slot, true);
  if (!equipped || uid == NULL) return;
  SCOPE_GUARD(g_lock);
  std::vector currentMaterials{config};
  ModState::GetSingleton()->VisitMaterials(uid, [&](const auto& name) {
    if (!std::ranges::any_of(currentMaterials, [name](MATC& matc) {
          return matc.name == name;
        })) {
      if (const auto& matc = GetMaterialConfig(equipped->GetFormID(), name);
          matc.has_value() && matc.value().layer != config.layer) {
        currentMaterials.emplace_back(matc.value());
      }
    }
    return BSVisitControl::kContinue;
  });
  auto* invItem = Helpers::GetInventoryItemWithUID(actor, uid);
  UpdateInventoryItemMaterials(uid, invItem->data.get(), currentMaterials);
  MeshBuilder::ApplyMaterialToRefr(actor, config);
}

void LoadArmorMaterials(RE::Actor* actor) {
  if (!actor) return;
  for (u8 i = 1; i < 32; ++i) {
    LoadArmorMaterials(actor, (RE::BipedObjectSlot)(1 << i));
  }
}

void LoadArmorMaterials(RE::Actor* actor, RE::BipedObjectSlot slot) {
  auto* equipped = actor->GetWornArmor(slot);
  auto uid = Helpers::GetUniqueID(actor, slot, false);
  if (!equipped || uid == NULL) return;
  SCOPE_GUARD(g_lock);
}

result<RE::NiNode*> RenderArmorMaterials(RE::Actor* actor,
                                         RE::BipedObjectSlot slot) {
  if (!actor) return Err{"Actor is null"};
  auto* equipped = actor->GetWornArmor(slot);
  auto uid = Helpers::GetUniqueID(actor, slot, false);
  if (!equipped || uid == NULL) return Ok{nullptr};
  SCOPE_GUARD(g_lock);
  RE::NiPointer<RE::NiNode> nif;
  if (const auto err =
          RE::BSModelDB::Demand(equipped->GetArmorAddon(actor->GetRace())
                                    ->bipedModels[RE::SEXES::kFemale]
                                    .GetModel(),
                                nif, RE::BSModelDB::DBTraits::ArgsType{});
      err != RE::BSResource::ErrorCode::kNone) {
    switch (err) {
      case RE::BSResource::ErrorCode::kBusy:
        return Err{"Failed to load NIF: DEVICE_BUSY"};
      case RE::BSResource::ErrorCode::kFileError:
        return Err{"Failed to load NIF: FILE_ERROR"};
      case RE::BSResource::ErrorCode::kInvalidParam:
        return Err{"Failed to load NIF: INVALID_PARAM"};
      case RE::BSResource::ErrorCode::kInvalidPath:
        return Err{"Failed to load NIF: INVALID_PATH"};
      case RE::BSResource::ErrorCode::kInvalidType:
        return Err{"Failed to load NIF: INVALID_TYPE"};
      case RE::BSResource::ErrorCode::kMemoryError:
        return Err{"Failed to load NIF: MEMORY_ERROR"};
      case RE::BSResource::ErrorCode::kNotExist:
        return Err{"Failed to load NIF: NOT_EXIST"};
      case RE::BSResource::ErrorCode::kUnsupported:
        return Err{"Failed to load NIF: UNSUPPORTED"};
      case RE::BSResource::ErrorCode::kNone:
        break;
    }
  }
  auto materials = ModState::GetSingleton()->GetMaterials(uid);
  auto* node = CloneWithMaterial(nif.get(), equipped->GetFormID(), materials);
  if (!node) return Err{"Failed to clone NiNode"};
  return Ok{node};
}

void VisitAppliedArmorMaterials(RE::Actor* actor, RE::InventoryEntryData* data,
                                const Visitor<const MATC&>& visitor) {
  const auto uid = Helpers::GetUniqueID(actor, data, false);
  if (uid == NULL) return;
  SCOPE_GUARD(g_lock);
  ModState::GetSingleton()->VisitMaterials(uid, [data,
                                                 visitor](const auto& name) {
    auto result = BSVisitControl::kContinue;
    if (const auto matc = GetMaterialConfig(data->object->GetFormID(), name);
        matc.has_value()) {
      result = visitor(matc.value());
    }
    return result;
  });
}

void VisitMaterialFiles(RE::FormID formID, const Visitor<MATC>& visitor) {
  FIND_IN(g_configs, it, formID) {
    for (const auto& matc : it->second | std::views::values) {
      if (visitor(matc) == BSVisitControl::kStop) {
        return;
      }
    }
  }
}

void VisitApplicableMaterials(const RE::TESForm* form,
                              const Visitor<MATC>& visitor) {
  if (!form) {
    // can happen if iterating over a non-equipped item?
    return;
  }
  VisitMaterialFiles(form->GetFormID(), visitor);
}

void VisitAppliedArmorMaterials(RE::Actor* actor, RE::BipedObjectSlot slot,
                                const Visitor<MATC>& visitor) {
  auto uid = Helpers::GetUniqueID(actor, slot, false);
  if (uid == NULL) {
    return;
  }
  VisitMaterials(uid, visitor);
}

void VisitAppliedArmorMaterials(RE::Actor* actor, RE::InventoryEntryData* data,
                                const Visitor<MATC>& visitor) {
  const auto uid = Helpers::GetUniqueID(actor, data, false);
  if (uid == NULL) {
    return;
  }
  VisitMaterials(uid, visitor);
}

void VisitAppliedWeaponMaterials(RE::Actor* actor, bool left,
                                 const Visitor<MATC>& visitor) {
  const auto* data = actor->GetEquippedEntryData(left);
  const auto uid = Helpers::GetUniqueID(actor, data, false);
  if (uid == NULL) {
    return;
  }
  VisitMaterials(uid, visitor);
}
}  // namespace MaterialSwapper