#pragma once

namespace Cond {
enum class ConditionType : uint8_t {
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
  kActorHasEquipped,
  kActorHasEquippedInSlot,

  // weather
  kWeatherEquals,
  kTimeOfDay,
};

enum class MathOperator : uint8_t {
  kNone,
  kEqual,
  kNotEqual,
  kGreaterThan,
  kLessThan,
  kGreaterThanOrEqual,
  kLessThanOrEqual,
};

struct Condition {
  ConditionType type = ConditionType::kNone;
  MathOperator op = MathOperator::kNone;
  bool negate;
};
}  // namespace Cond