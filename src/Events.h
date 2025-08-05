#pragma once

#include "Events/PlayerEvents.h"
#include "Events/PapyrusEvents.h"
#include "Events/NiNodeUpdateEvents.h"
#include "Events/CellEvents.h"

namespace Events {
inline void Configure() noexcept {
  PlayerEvents::Configure();
  PapyrusEvents::Configure();
  NiNodeUpdateEvents::Configure();
  // CellEvents::Configure();
}
}
