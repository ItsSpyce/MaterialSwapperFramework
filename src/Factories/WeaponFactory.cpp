#include "WeaponFactory.h"

#include "Helpers.h"

namespace Factories {
bool WeaponFactory::ApplyMaterial(RE::Actor* actor, bool leftHand,
                                  const MaterialConfig& material) {
  RETURN_IF_FALSE(actor)
  auto* weapon = actor->GetEquippedObject(leftHand)->As<RE::TESObjectWEAP>();
  RETURN_IF_FALSE(weapon)
  auto* weaponData = actor->GetEquippedEntryData(leftHand);
  RETURN_IF_FALSE(weaponData)
  return false;
}

bool WeaponFactory::ApplyDefaultMaterial(RE::Actor* actor, bool leftHand) {
  // Implementation for applying default material to weapon
  return true;
}

bool WeaponFactory::ApplySavedMaterial(RE::Actor* actor, bool leftHand) {
  // Implementation for applying saved material to weapon
  return true;
}
}  // namespace Factories