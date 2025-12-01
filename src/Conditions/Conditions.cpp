#include "Conditions.h"

#include "EditorIDCache.h"
#include "Helpers.h"
#include "NiOverride.h"

namespace Conditions {
void Condition::Render() const {
  // TODO:
}

bool Condition::EvaluateImpl(RE::TESObjectREFR* refr) const {
  if (type.empty()) {
    _WARN("Condition type is empty");
    return false;
  }
  if (type == "editorId") {
    const string editorID = value.get_string();
    _DEBUG("Comparing {} to {}", editorID, refr->GetFormEditorID());
    return EditorIDCache::GetEditorID(refr) == editorID;
  }
  if (type == "formId") {
    RE::FormID formID = 0;
    if (value.is_string()) {
      formID = Helpers::GetFormID(value.get_string());
    } else if (value.is_number()) {
      formID = value.get_number();
    }
    return formID == refr->GetFormID();
  }
  if (type == "dayOfWeek") {
    const u32 dayOfWeek = value.get_number();
    return dayOfWeek == RE::Calendar::GetSingleton()->GetDayOfWeek();
  }
  if (type == "weather") {
    const auto* cell = refr->GetParentCell();
    if (!cell->IsExteriorCell() && !cell->UsesSkyLighting()) {
      _TRACE("Cell is not using sky lighting");
      return false;
    }
    RE::TESWeather::WeatherDataFlag lookingFor;
    auto weather = StringHelpers::ToLower(value.get_string());
    if (weather == "pleasant") {
      lookingFor = RE::TESWeather::WeatherDataFlag::kPleasant;
    } else if (weather == "cloudy") {
      lookingFor = RE::TESWeather::WeatherDataFlag::kCloudy;
    } else if (weather == "rainy") {
      lookingFor = RE::TESWeather::WeatherDataFlag::kRainy;
    } else if (weather == "snow") {
      lookingFor = RE::TESWeather::WeatherDataFlag::kSnow;
    } else {
      return false;
    }
    RE::TESWeather* currentWeather;
    auto* sky = RE::Sky::GetSingleton();
    if (sky->overrideWeather) {
      currentWeather = sky->overrideWeather;
    } else if (sky->currentWeather) {
      currentWeather = sky->currentWeather;
    } else {
      currentWeather = sky->defaultWeather;
    }
    if (!currentWeather) {
      return false;
    }
    return currentWeather->data.flags.any(lookingFor);
  }
  if (type == "race") {
    if (auto* actor = refr->As<RE::Actor>()) {
      auto strValue = value.get_string();
      if (strValue == actor->GetRace()->GetFormEditorID()) {
        return true;
      }
      if (auto formID = Helpers::GetFormID(strValue)) {
        return formID == actor->GetRace()->GetFormID();
      }
    }
    return false;
  }
  if (type == "morph") {
    // format is "keyName;morphName=|<|>" or "morphName=|<|>"
    const auto condition = value.get_string();
    string keyName, morphName, comparator, comparison;
    auto comparatorPos = StringHelpers::GetPosForOneOf(condition, "=<>");
    if (auto pos = condition.find(';'); pos != string::npos) {
      keyName = condition.substr(0, pos);
      morphName = condition.substr(pos + 1, comparatorPos - pos);
    } else {
      keyName = "RSMLegacy";
      morphName = condition.substr(0, comparatorPos);
    }
    if (comparatorPos != string::npos) {
      comparator = condition.at(comparatorPos);
      comparison = condition.substr(comparatorPos + 1);
    }
    _DEBUG("Key: {}, morph: {}, comparator: {}, comparison: {}", keyName, morphName, comparator, comparison);
    if (morphName.empty() || comparator.empty() || comparison.empty()) {
      return false;
    }
    auto bodyMorphVal =
        ceil(NiOverride::GetBodyMorph()(RE::StaticFunctionTag{}, refr,
                                        morphName.c_str(), keyName.c_str()) *
             100.0) /
        100.0;  // round to 2 places
    return bodyMorphVal == stof(comparison);
  }
  if (type == "hasmorph") {
    const auto condition = value.get_string();
    string keyName, morphName;
    if (auto pos = condition.find(';'); pos != string::npos) {
      keyName = condition.substr(0, pos);
      morphName = condition.substr(pos + 1);
    } else {
      keyName = "RSMLegacy";
      morphName = condition;
    }
    return NiOverride::HasBodyMorph()(RE::StaticFunctionTag{}, refr,
                                      morphName.c_str(), keyName.c_str());
  }
  return false;
}
}  // namespace Conditions