#pragma once

#include <glaze/glaze.hpp>

namespace Translations {
inline result<std::string> GetTranslation(const char* key) {
  static std::unordered_map<std::string, std::string> translations;
  if (translations.empty()) {
    if (const auto err = glz::read_file_jsonc(translations, "Data/interface/MSF_english.json", std::string{})) {
      return Err{"Failed to read translations file: {}", glz::format_error(err)};
    }
  }
  FIND_IN(translations, it, key) {
    return Ok{it->second};
  }
  return Err{"Translation key not found for {}", key};
}
}

static const char* operator""_tr(const char* str, std::size_t) {
  if (auto translation = Translations::GetTranslation(str)) {
    return translation.value().c_str();
  } else {
    _ERROR("Failed to get translation: {}", translation.error());
    return str;
  }
}