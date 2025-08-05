#pragma once

#include "Hooks/UpdatePlayerHook.h"
#include "Hooks/CacheEditorIDsHook.h"
#include "Hooks/Load3DHook.h"

namespace Hooks {

inline void Install() noexcept {
  // stl::write_vfunc<RE::PlayerCharacter, UpdatePlayerHook>();
  stl::write_vfunc<RE::TESObjectARMO, SetFormEditorIDHook>();
}
}