#pragma once

#include "Hooks/UpdatePlayerHook.h"

namespace Hooks {

inline void Install() noexcept {
  stl::write_vfunc<RE::PlayerCharacter, UpdatePlayerHook>();
}
}