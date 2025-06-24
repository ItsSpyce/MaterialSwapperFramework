#pragma once

#include "Events/PlayerEvents.h"
#include "Events/PapyrusEvents.h"

namespace Events {
inline void Configure() noexcept {
  PlayerEvents::Configure();
  PapyrusEvents::Configure();
}
}
