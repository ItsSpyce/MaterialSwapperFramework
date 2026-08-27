#pragma once

#include <glaze/glaze.hpp>

#include "Core/StringReader.h"

namespace Translations {
inline result<std::string> GetTranslation(const char* key) {
  static std::unordered_map<std::string, std::string> translations;
  if (translations.empty()) {
    if (const auto err =
            glz::read_file_jsonc(translations,
                                "Data/interface/MaterialSwapperFramework/"
                                "translations/MSF_english.json",
                                std::string{})) {
      return Err{"{}", glz::format_error(err)};
    }
  }
  const Core::StringReader reader{key};
  std::string result;
  _DEBUG("Reading translation");
  while (!reader.AtEnd()) {
    auto word = reader.ReadUntil(' ');
    if (word[0] == '$') {
      FIND_IN(translations, it, word) {
        _DEBUG("\tTR: {} -> {}", key, it->second);
        result += fmt::format(" {}", it->second);
        continue;
      }
      else {
        _WARN("Missing translation: {}", it->second);
      }
    }
    result += fmt::format(" {}", word);
    reader.Skip(1);
  }
  _DEBUG("\tTR: {} -> {}", key, result);
  return Ok{result};
}
}  // namespace Translations

static const char* operator""_tr(const char* str, std::size_t) {
  if (auto translation = Translations::GetTranslation(str)) {
    return translation.value().c_str();
  } else {
    _ERROR("Failed to get translation: {}", translation.error());
    return str;
  }
}