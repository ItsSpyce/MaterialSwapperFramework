#pragma once

struct MaterialRecord {
  std::string name;
  std::string filename;
  std::vector<std::string> shapes{};
  bool isHidden;
  bool modifyName = true;
};

struct FormIDConfigEntry {
  std::vector<MaterialRecord> materials{};
};

using MaterialConfig = std::unordered_map<std::string, std::vector<MaterialRecord>>;