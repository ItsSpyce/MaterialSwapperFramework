#pragma once

#include "Events/PlayerEquipEvent.h"
#include "Events/PapyrusEvent.h"
#include "Events/NiNodeUpdateEvent.h"
#include "Events/InventoryOpenEvent.h"

namespace Events {
inline void Configure() noexcept {
  PlayerEquipEvent::Configure();
  PapyrusEvent::Configure();
  NiNodeUpdateEvent::Configure();
  InventoryOpenEvent::Configure();
}
}
