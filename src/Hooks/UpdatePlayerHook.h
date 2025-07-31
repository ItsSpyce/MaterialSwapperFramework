#pragma once

#include "Factories.h"

namespace Hooks {
struct UpdatePlayerHook {
  static void thunk(RE::PlayerCharacter* player, float delta) {
    func(player, delta);
    Factories::ArmorFactory::GetSingleton()->OnUpdate();
  }

  static inline REL::Relocation<decltype(thunk)> func;
  static inline auto idx = 0xAD;
};
}