#pragma once

// from https://github.com/powerof3/po3-Tweaks/blob/master/src/Cache.h

namespace EditorIDCache {
class EditorID : public Singleton<EditorID> {
 public:
  void CacheEditorID(RE::FormID formID, const char* editorID) {
    Locker locker(lock_);
    formIDToEditorIDMap_.try_emplace(formID, editorID);
    editorIDToFormIDMap_.try_emplace(editorID, formID);
  }
  void CacheEditorID(const RE::TESForm* form, const char* editorID) {
    CacheEditorID(form->GetFormID(), editorID);
  }

  const std::string& GetEditorID(RE::FormID formID) {
    if (const auto it = formIDToEditorIDMap_.find(formID);
        it != formIDToEditorIDMap_.end()) {
      return it->second;
    }

    static std::string emptyStr;
    return emptyStr;  // Return empty string if not found
  }
  const std::string& GetEditorID(const RE::TESForm* form) {
    return GetEditorID(form->GetFormID());
  }
  RE::FormID GetFormID(const std::string& editorID) {
    if (const auto it = editorIDToFormIDMap_.find(editorID);
        it != editorIDToFormIDMap_.end()) {
      return it->second;
    }
    return 0;  // Return 0 if not found
  }

 private:
  using Lock = std::mutex;
  using Locker = std::scoped_lock<Lock>;

  mutable Lock lock_;
  std::unordered_map<RE::FormID, std::string> formIDToEditorIDMap_;
  std::unordered_map<std::string, RE::FormID> editorIDToFormIDMap_;
};

inline const std::string& GetEditorID(RE::FormID formID) {
  return EditorID::GetSingleton()->GetEditorID(formID);
}
inline const std::string& GetEditorID(const RE::TESForm* form) {
  return EditorID::GetSingleton()->GetEditorID(form);
}
inline RE::FormID GetFormID(const std::string& editorID) {
  return EditorID::GetSingleton()->GetFormID(editorID);
}
inline void HydrateEditorIDCache() {
  const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
  const RE::BSWriteLockGuard locker{lock};
  for (const auto& [editorID, form] : *map) {
    switch (form->GetFormType()) {
      case RE::FormType::Armor:
      case RE::FormType::NPC:
      case RE::FormType::Weapon:
        EditorID::GetSingleton()->CacheEditorID(form, editorID.c_str());
        break;
      default:
        break;
    }
  }
}
}  // namespace EditorIDCache