#include "MaterialSwapper.h"

#include "Cache/FilenameIDCache.h"
#include "Core/MaterialScope.h"
#include "Filesystem.h"
#include "Graphics/MaterialManager.h"
#include "Helpers/JsonHelpers.h"
#include "Helpers/MaterialHelpers.h"
#include "Helpers/RaceMenuHelpers.h"
#include "Helpers/SkyrimHelpers.h"
#include "ModState.h"
#include "Result.h"
#include "emhash/hash_table5.hpp"

namespace {
namespace fs = std::filesystem;
using RE::BSVisit::BSVisitControl;

std::mutex g_lock;
emhash5::HashMap<RE::FormID, emhash8::HashMap<string, MATC>> g_configs;
emhash5::HashMap<FileID, MATR> g_records;
consteval size_t MAX_SEARCH_HISTORY = 16;
consteval auto DEFAULT_MATERIAL_KEY = "__DEFAULT__";

template <typename T>
std::optional<T> RecursivelyFind(
    const MATR& record,
    const std::function<std::optional<T>(const MATR& record)>& find) {
  std::vector<FileID> history(MAX_SEARCH_HISTORY);
  std::optional<T> result = std::nullopt;
  auto current = record.id;
  while (!result.has_value() && current != 0 &&
         // prevent an overflow, probably a better way to do it but fuck it
         !std::ranges::contains(history, current)) {
    // creating a ref here is not supported. per emhash docs, there's no
    // reference stability guarantee so we can NOT make edits to a record, lest
    // a rehash occur
    const auto& parent = g_records[record.inherits];
    result = find(parent);
    current = parent.id;
  }
  return result;
}

result<MATR> ReadMaterialJson(const std::string& filename) noexcept {
  glz::generic json;
  MATR record;
  if (!fs::exists(filename)) {
    return Err<MATR>("Material file does not exist: {}", filename);
  }
  record.id = FilenameIDCache::GetFilenameID(filename);
  if (const auto it = g_records.find(record.id); it != g_records.end()) {
    return Ok(it->second);
  }
  if (auto err = glz::read_file_jsonc(json, filename, string{})) {
    auto cleanedErr = glz::format_error(err);
    return Err<MATR>("Failed to read material file {}: {}", filename,
                     cleanedErr);
  }
  if (auto inherits = JsonHelpers::MaybeGet<std::string>(json, "inherits")) {
    auto parent = ReadMaterialJson(inherits.value());
    if (parent.has_value()) {
      record.inherits = parent->id;
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
  auto uvOffset =
      JsonHelpers::MaybeGet<array<half, 2>>(json, "uvOffset").value_or({0, 0});
  auto uvScale =
      JsonHelpers::MaybeGet<array<half, 2>>(json, "uvScale").value_or({1, 1});
  record.uv = uv(uvScale, uvOffset);
  record.clamp = JsonHelpers::MaybeGet<u8>(json, "clamp");
  record.transparency =
      JsonHelpers::MaybeGet<half>(json, "transparency")
          .transform([](const float v) { return static_cast<u8>(v * 255); });
  record.sourceBlendMode = JsonHelpers::MaybeGet<u8>(json, "sourceBlendMode");
  record.destinationBlendMode =
      JsonHelpers::MaybeGet<u8>(json, "destinationBlendMode");
  record.alphaTestThreshold =
      JsonHelpers::MaybeGet<float>(json, "alphaTestThreshold")
          .transform([](const float v) { return static_cast<u8>(v * 255); });
  record.maskWrites = JsonHelpers::MaybeGet<u8>(json, "maskWrites");
  record.refractionPower = JsonHelpers::MaybeGet<half>(json, "refractionPower");
  record.envMapMaskScale = JsonHelpers::MaybeGet<half>(json, "envMapMaskScale");
  record.rimPower = JsonHelpers::MaybeGet<half>(json, "rimPower");
  record.backLightPower = JsonHelpers::MaybeGet<half>(json, "backLightPower");
  record.specularPower = JsonHelpers::MaybeGet<half>(json, "specularPower");
  record.subsurfaceLightingRolloff =
      JsonHelpers::MaybeGet<half>(json, "subsurfaceLightingRolloff");
  record.specularColor =
      JsonHelpers::MaybeGet<array<u8, 3>>(json, "specularColor");
  record.specularMult = JsonHelpers::MaybeGet<half>(json, "specularMult");
  record.smoothness = JsonHelpers::MaybeGet<half>(json, "smoothness");
  record.fresnelPower = JsonHelpers::MaybeGet<half>(json, "fresnelPower");
  record.emitColor = JsonHelpers::MaybeGet<array<u8, 3>>(json, "emitColor");
  record.emitMult = JsonHelpers::MaybeGet<half>(json, "emitMult");
  record.colorBlendMode =
      JsonHelpers::MaybeGet<u8>(json, "colorBlendMode")
          .transform([](const u8 v) { return (ColorBlendMode)v; });
  record.colorChannelR =
      JsonHelpers::MaybeGet<array<u8, 4>>(json, "colorChannelR");
  record.colorChannelG =
      JsonHelpers::MaybeGet<array<u8, 4>>(json, "colorChannelG");
  record.colorChannelB =
      JsonHelpers::MaybeGet<array<u8, 4>>(json, "colorChannelB");
  return Ok(record);
}

result<std::vector<MATC>> ReadConfigJson(const std::string& filename) {
  std::vector<MATC> matConfigs;
  if (filename[0] == '_') return Ok(matConfigs);
  std::unordered_map<std::string, std::vector<glz::generic>> json;
  _DEBUG("Reading material config {}", filename);
  if (auto err = glz::read_file_json(json, filename, "")) {
    return Err<std::vector<MATC>>(
        "Failed to read material config file at {}:\r\n{}", filename,
        glz::format_error(err));
  }
  for (const auto& [formID, configs] : json) {
    for (const auto& config : configs) {
      MATC item;
      item.form = Helpers::GetFormID(formID);
      if (item.form == 0) {
        _ERROR("Unknown form ID {}", item.form);
        continue;
      }
      item.name = config["name"].get_string();
      item.isHidden =
          JsonHelpers::MaybeGet<bool>(config, "isHidden").value_or(false);
      item.modifyName =
          JsonHelpers::MaybeGet<bool>(config, "modifyName").value_or(true);
      item.layer = JsonHelpers::MaybeGet<u8>(config, "layer").value_or(0);
      item.gender = JsonHelpers::MaybeGet<u8>(config, "gender")
                        .value_or(RE::SEXES::kTotal);
      auto conditions =
          JsonHelpers::MaybeGet<std::vector<glz::generic>>(config, "conditions")
              .value_or({});
      for (const auto& condition : conditions) {
        MaterialCondition materialCondition;
        // legacy condition handling
        if (condition.contains("type")) {
          _DEBUG("Converting legacy condition");
          switch (condition["type"].get_string()) {
            case "dayOfWeek":
              materialCondition.function = MaterialFunctionID::GetDayOfWeek;
              break;
            case "weather":
              materialCondition.function =
                  MaterialFunctionID::GetIsCurrentWeather;
              break;
            default:
              break;
          }

          materialCondition.params = {
              condition["value"].get<MaterialConditionParam>()};
        } else {
          materialCondition = condition.as<MaterialCondition>();
        }
        item.conditions.emplace_back(materialCondition);
      }
      const auto applies =
          JsonHelpers::MaybeGet<std::unordered_map<std::string, std::string>>(
              config, "applies")
              .value_or({});
      for (const auto& [shape, filename] : applies) {
        const auto path =
            StringHelpers::AssertPrefix(filename, "Data\\materials\\");
        const auto matr = ReadMaterialJson(path);
        if (matr.has_value()) {
          const auto filenameID = FilenameIDCache::GetFilenameID(path);
          g_records[filenameID] = matr.value();
          item.applies.emplace_back(std::tuple(shape, filenameID));
        }
      }
      matConfigs.emplace_back(item);
    }
  }
  return Ok(matConfigs);
}

RE::NiNode* CloneWithMaterial(RE::NiNode* node, RE::FormID formId,
                              const vector<string>& materials) {
  auto* clone = (RE::NiNode*)node->Clone();
  RE::BSVisit::TraverseScenegraphObjects(clone, [&](RE::NiAVObject* geometry) {
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
      const auto matr = MaterialSwapper::GetMaterialRecord(materialFileID);
      if (matr) {
        MaterialManager::ApplyMaterialToNode(ts, matr.value());
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
          _ERROR("Failed to load material record {}", materialPath);
          continue;
        }
        MaterialScope scope(matc, matr);
        _TRACE("Applying saved material {}", materialName);
        MaterialManager::ApplyMaterialToNode(
            ts, matr,
            MaterialHelpers::GetMaterialShapeKey(formId, materialPath.value(),
                                                 materialName));
      }
    }
    return BSVisitControl::kContinue;
  });
  return clone;
}

void UpdateInventoryItemMaterials(const UniqueID uid,
                                  RE::InventoryEntryData* data,
                                  const std::vector<MATC>& materials) {
  ModState::GetSingleton()->SetMaterials(
      uid, materials | views::transform([](auto& matc) { return matc.name; }) |
               ranges::to<std::vector>());
  Helpers::ResetDisplayName(data);

  const auto name =
      fmt::format("{} [{}]", data->GetDisplayName(),
                  StringHelpers::Join(materials | views::filter([](auto& matc) {
                                        return matc.modifyName;
                                      }) | views::transform([](auto& matc) {
                                        return matc.name;
                                      }) | ranges::to<std::vector>(),
                                      ", "));
  Helpers::SetItemDisplayName(data, name.c_str());
}
}  // namespace

namespace MaterialSwapper {
void ReadMaterialConfigurations() {
  SCOPE_GUARD(g_lock);
  g_configs.clear();
  // this is faster than for-each over everything apparently?
  const auto globalRdi = Filesystem::EnumerateMaterialConfigDir();
  FOR_IN_DIR(globalRdi) {
    const auto configs = ReadConfigJson(it->path().string());
    if (!configs.has_value()) {
      _ERROR("Failed to read config file {}: {}", it->path(),
             configs.error().what());
      continue;
    }
    for (const auto& config : configs.value()) {
      // you might call me dumb for this but emhash doesn't support
      // refing a value then modifying. I mean, it _does_ but it has
      // to rehash which is more expensive than just getting and then
      // upserting
      auto& [formConfigs, _] =
          g_configs.do_insert(config.form, emhash8::HashMap<string, MATC>{});
      formConfigs->second.emplace(config.name, config);
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
      for (const auto& config : configs.value()) {
        auto& [formConfigs, _] =
            g_configs.do_insert(config.form, emhash8::HashMap<string, MATC>{});
        formConfigs->second.emplace(config.name, config);
      }
    }
  }
}

void VisitMaterialFiles(RE::FormID formID,
                        const Visitor<const MATC&>& visitor) {
  if (formID == NULL) return;
  FIND_IN(g_configs, it, formID) {
    for (const auto& config : it->second | views::values) {
      if (const auto control = visitor(config);
          control == BSVisitControl::kStop)
        return;
    }
  }
}

_NODISCARD std::optional<const MATR&> GetMaterialRecord(FileID fileID) {
  if (fileID == NULL) return std::nullopt;
  FIND_IN(g_records, it, fileID) { return it->second; }
  return std::nullopt;
}

_NODISCARD std::optional<const MATC&> GetMaterialConfig(
    RE::FormID formID, const std::string& name) {
  if (formID == NULL) return std::nullopt;
  FIND_IN(g_configs, it, formID) {
    FIND_IN(it->second, mIt, name) { return mIt->second; }
  }
  return std::nullopt;
}

_NODISCARD std::optional<const MATC&> GetDefaultMaterialConfig(
    RE::FormID formID) {
  return GetMaterialConfig(formID, DEFAULT_MATERIAL_KEY);
}

_NODISCARD bool HasMaterialConfigs(RE::FormID formID) {
  if (formID == NULL) return false;
  return g_configs.contains(formID);
}

void ResetAll() { SCOPE_GUARD(g_lock); }

void VisitApplicableMaterials(const RE::TESForm* form,
                              const Visitor<const MATC&>& visitor) {
  FIND_IN(g_configs, it, form->GetFormID()) {
    for (const auto& matc : it->second | views::values) {
      if (visitor(matc) == BSVisitControl::kStop) {
        return;
      }
    }
  }
}

void ResetEquippedArmors(RE::Actor* actor) {
  for (u8 i = 1; i < 32; ++i) {
    ResetEquippedArmor(actor, (RE::BipedObjectSlot)(1 << i));
  }
}

void ResetEquippedArmor(RE::Actor* actor, RE::BipedObjectSlot slot) {
  auto* equipped = actor->GetWornArmor(slot);
  auto uid = Helpers::GetUniqueID(actor, slot, false);
  if (!equipped || uid == NULL) return;
  SCOPE_GUARD(g_lock);
}

void ApplyArmorMaterial(RE::Actor* actor, RE::BipedObjectSlot slot,
                        const MATC& config) {
  auto* equipped = actor->GetWornArmor(slot);
  auto uid = Helpers::GetUniqueID(actor, slot, true);
  if (!equipped || uid == NULL) return;
  SCOPE_GUARD(g_lock);
  std::vector currentMaterials{config};
  ModState::GetSingleton()->VisitMaterials(uid, [&](const auto& name) {
    if (!std::ranges::contains(currentMaterials, name)) {
      if (const auto& matc = GetMaterialConfig(equipped->GetFormID(), name);
          matc.has_value() && matc.value().layer != config.layer) {
        currentMaterials.emplace_back(matc);
      }
    }
    return BSVisitControl::kContinue;
  });
  auto* invItem = Helpers::GetInventoryItemWithUID(actor, uid);
  UpdateInventoryItemMaterials(uid, invItem->data.get(), currentMaterials);
  MaterialManager::ApplyMaterialToRefr(actor, config);
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
}  // namespace MaterialSwapper