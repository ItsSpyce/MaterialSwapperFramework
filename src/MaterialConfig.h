#pragma once

struct MaterialRecord {
  std::string name;
  std::unordered_map<std::string, std::string> applies;
  bool isHidden;
  bool modifyName = true;
};

struct FormIDConfigEntry {
  std::vector<MaterialRecord> materials{};
};

using MaterialConfig = std::unordered_map<std::string, std::vector<MaterialRecord>>;