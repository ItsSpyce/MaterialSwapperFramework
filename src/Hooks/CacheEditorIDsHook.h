#pragma once

#include "EditorIDCache.h"

namespace Hooks {
struct SetFormEditorIDHook {
  static bool thunk(RE::TESForm* form, const char* editorID) {
    if (std::strlen(editorID) > 0 && !form->IsDynamicForm()) {
      const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
      const RE::BSWriteLockGuard locker{lock};
      if (map) {
        map->emplace(editorID, form);
      }
      EditorIDCache::EditorID::GetSingleton()->CacheEditorID(form, editorID);
    }
    return func(form, editorID);
  }

  static inline REL::Relocation<decltype(&thunk)> func;
  static constexpr size_t idx{0x33};
};
}