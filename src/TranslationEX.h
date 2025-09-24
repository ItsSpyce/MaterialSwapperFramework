#pragma once

#include <glaze/glaze.hpp>

template <typename T>
concept translation = requires(T t) {
  { t.value } -> std::convertible_to<std::string>;
  { t.name } -> std::convertible_to<std::string>;
};

#define TRANSLATION_KEY(_NAME) \
  static inline TranslationEX::TranslationKey _NAME{.name = #_NAME};

class TranslationEX {
 public:
  struct TranslationKey {
    string name;
    string value;

    const char* operator()() {
      return TryTranslate(*this);
    }
  };
  static void UsePluginName(const std::string& pluginName) {
    pluginName_ = pluginName;
    ReadSettings();
  }

  static void UseDirectory(const std::string& directory) {
    directory_ = directory;
    ReadSettings();
  }

  template <translation Key>
  static const char* TryTranslate(Key& key) {
    if (!key.value.empty()) {
      return key.value.c_str();
    }
    if (translationMap_.empty()) {
      ReadTranslations();
    }
    if (auto it = translationMap_.find(key.name); it != translationMap_.end()) {
      std::string val = it->second;
      // do replace on all keys prefixed with $
      size_t pos = 0;
      while ((pos = val.find('$', pos)) != std::string::npos) {
        auto endPos = val.find(' ', pos);
        if (endPos == std::string::npos) {
          endPos = val.length();
        }
        const auto keyName = val.substr(pos + 1, endPos - pos - 1);
        TranslationKey subKey{.name = keyName};
        TryTranslate(subKey);
        val.replace(pos, endPos - pos, subKey.value);
        pos += subKey.value.length();
      }
      key.value = std::string(val);
      return key.value.c_str();
    }
    return key.name.c_str();
  }

 private:
  static inline string_view directory_ = "interface/translations"sv;
  static inline std::string currentLanguage_;
  static inline std::unordered_map<string, string> translationMap_;
  static inline auto pluginName_ = std::string(SKSE::GetPluginName());

  static void ReadSettings() {
    static bool hasRead = false;
    if (hasRead) {
      return;
    }
    hasRead = true;
    const auto iniSettingCollection = RE::INISettingCollection::GetSingleton();
    auto setting = iniSettingCollection
                       ? iniSettingCollection->GetSetting("sLanguage:General")
                       : nullptr;
    currentLanguage_ =
        setting && setting->GetType() == RE::Setting::Type::kString
            ? setting->data.s
            : "ENGLISH"s;
  }

  static void ReadTranslations() {
    auto dir = std::filesystem::path("Data") / directory_;
    auto translationFile =
        (dir / fmt::format("{}_{}.json", pluginName_,
                           glz::to_lower_case(currentLanguage_)))
            .string();
    if (!std::filesystem::exists(translationFile)) {
      _WARN("Translation file does not exist: {}", translationFile);
      return;
    }
    std::unordered_map<string, string> map;
    if (auto err = glz::read_file_json(map, translationFile, std::string{})) {
      auto cleanedError = glz::format_error(err);
      _ERROR("Failed to read translation file {}: {}", translationFile,
             cleanedError);
      return;
    }
    translationMap_ = std::move(map);
    _INFO("Loaded translations for language: {}", currentLanguage_);
    _TRACE("Translations: {}", translationMap_.size());
    for (const auto& [key, value] : translationMap_) {
      _TRACE("  {} => {}", key, value);
    }
  }
};