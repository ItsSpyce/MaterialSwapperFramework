#pragma once

#include "Conditions/Condition.h"

struct MaterialConfig {
  string name;
  unordered_map<string, string> applies;
  bool isHidden;
  bool modifyName = true;
  uint8_t layer = 0;
  vector<Conditions::Condition> conditions;
};

struct FormIDConfigEntry {
  vector<MaterialConfig> materials{};
};

using MaterialConfigMap = unordered_map<string, vector<MaterialConfig>>;