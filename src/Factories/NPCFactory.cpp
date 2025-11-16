#include "NPCFactory.h"

namespace Factories {
bool NPCFactory::ApplyDefaultMaterial(RE::Actor* npc) {
  return false;
}

bool NPCFactory::ApplyMaterial(RE::Actor* npc, const MaterialConfig& material) {
  return false;
}

bool NPCFactory::ApplySavedMaterial(RE::Actor* npc) {
  return false;
}

}