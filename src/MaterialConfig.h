#pragma once

struct ArmorMaterialConfig {
  std::string name;
  std::vector<std::string> shapes{};
  std::string filename;
  bool hidden;
};

struct ArmorConfig {
  std::vector<ArmorMaterialConfig> materials{};
};

struct MaterialConfig {
  std::unordered_map<std::string, ArmorConfig> armors{};
};