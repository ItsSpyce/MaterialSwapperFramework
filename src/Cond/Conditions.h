#pragma once

namespace Cond {
enum ConditionType : uint8_t {
  kNone,
  // actor specific
  kActorIsInCombat,
  kActorIsSneaking,
  kActorIsWalking,
  kActorIsRunning,
  kActorIsSprinting,
  kActorIsSitting,
  kActorIsInWater,
  kActorIsWeaponDrawn,
  kActorIsSwingingWeapon,
  kActorIsWeaponSwung,
  kActorIsBlocking,
  kActorIsBowDrawn,
  kActorIsBowFired,
  kActorIsMagicCasting,
  kActorIsMagicCast,
  kActorIsAnimating,
  kActorIsCrafting,
  kActorIsInDialogue,
  kActorHasEffect,
  kActorHasEnchantment,
  kActorHasSpell,
  kActorHasPerk,
  kActorHasKeyword,
  kActorHasFaction,
  kActorHasRace,
  kActorHasClass,

};
}