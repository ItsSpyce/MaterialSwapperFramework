#pragma once

struct MaterialConfig {
  std::string name;
  std::unordered_map<std::string, std::string> applies;
  bool isHidden;
  bool modifyName = true;
  uint8_t layer = 0;
};

struct FormIDConfigEntry {
  std::vector<MaterialConfig> materials{};
};

using MaterialConfigMap = std::unordered_map<std::string, std::vector<MaterialConfig>>;