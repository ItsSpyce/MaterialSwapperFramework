#pragma once

#include "EditorIDCache.h"
#include "Factories/ArmorFactory.h"

namespace Hooks {
struct TESObjectARMO_SetFormEditorID {
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

struct PlayerCharacter_Update {
  static void thunk(RE::PlayerCharacter* player, float delta) {
    func(player, delta);
    Factories::ArmorFactory::GetSingleton()->OnUpdate();
  }

  static inline REL::Relocation<decltype(thunk)> func;
  static inline auto idx = 0xAD;
};

inline void Install() noexcept {
  stl::write_vfunc<RE::PlayerCharacter, PlayerCharacter_Update>();
  stl::write_vfunc<RE::TESObjectARMO, TESObjectARMO_SetFormEditorID>();
}
}