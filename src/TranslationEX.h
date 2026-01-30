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
#define _S(_LITERAL) (const char*)u8##_LITERAL
  using hash_map = emhash8::HashMap<std::string, std::string>;
 public:
  struct TranslationKey {
    string name;
    string value;

    const char* operator()() { return TryTranslate(*this); }
  };
  static void UsePluginName(const std::string& pluginName) {
    pluginName_ = pluginName;
    ReadSettings();
  }

  static void UseDirectory(const std::string& directory) {
    directory_ = directory;
    ReadSettings();
  }

  static void DefineVariable(const std::string& key, const std::string& value) {
    translationMap_[key] = value;
  }

  template <translation Key>
  static const char* TryTranslate(Key& key) {
    if (!key.value.empty()) {
      return key.value.c_str();
    }
    if (!isLoaded_) {
      _TRACE("Reading translations");
      ReadTranslations();
    }
    if (translationMap_.try_get(key.name, key.value)) {
      // do replace on all keys prefixed with $
      size_t pos = 0;
      while ((pos = key.value.find('$', pos)) != std::string::npos) {
        auto endPos = key.value.find(' ', pos);
        if (endPos == std::string::npos) {
          endPos = key.value.length();
        }
        const auto keyName = key.value.substr(pos + 1, endPos - pos - 1);
        TranslationKey subKey{.name = keyName};
        TryTranslate(subKey);
        key.value.replace(pos, endPos - pos, subKey.value);
        pos += subKey.value.length();
      }
      _TRACE("Key: {}, Value: {}", key.name, key.value);
      return key.value.c_str();
    }
    return key.name.c_str();
  }

 private:
  static inline auto directory_ = "interface/translations"sv;
  static inline std::string currentLanguage_;
  static inline hash_map translationMap_;
  static inline auto pluginName_ = std::string(SKSE::GetPluginName());
  static inline bool isLoaded_ = false;

  static void ReadSettings() {
    static bool hasRead = false;
    if (hasRead) {
      return;
    }
    hasRead = true;
    auto* iniSettingCollection = RE::INISettingCollection::GetSingleton();
    auto* setting = iniSettingCollection
                        ? iniSettingCollection->GetSetting("sLanguage:General")
                        : nullptr;
    currentLanguage_ =
        setting && setting->GetType() == RE::Setting::Type::kString
            ? setting->data.s
            : "ENGLISH"s;
  }

  static void ReadTranslations() {
    isLoaded_ = true;
    const auto dir = std::filesystem::path("Data") / directory_;
    auto translationFile =
        (dir / fmt::format("{}_{}.json", pluginName_,
                           glz::to_lower_case(currentLanguage_)))
            .string();
    if (!std::filesystem::exists(translationFile)) {
      _WARN("Translation file does not exist: {}", translationFile);
      return;
    }
    string buffer;
    std::unordered_map<std::string, std::string> translations;
    if (auto err = glz::read_file_json(translations, translationFile, buffer)) {
      auto cleanedError = glz::format_error(err);
      _ERROR("Failed to read translation file {}: {}", translationFile,
             cleanedError);
      return;
    }
    translationMap_ = hash_map(translations.begin(), translations.end());
    _INFO("Loaded translations for language: {}", currentLanguage_);
    _TRACE("Translations: {}", translationMap_.size());
    for (const auto& [key, value] : translationMap_) {
      _TRACE("  {} => {}", key, value);
    }
  }
#undef _S
};