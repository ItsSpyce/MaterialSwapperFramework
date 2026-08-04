#pragma once

#include <half/half.h>

#include <magic_enum/magic_enum.hpp>

#include "Macros.h"
#include "STL.h"

// Definitions
#define opt std::optional
using IStreamPtr = std::unique_ptr<std::istream>;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = f32;
using half = FLOAT16;

typedef u16 FileID;
typedef u32 UniqueID;

enum class ColorBlendMode : u8;
enum class MaterialFlags : u32;
enum class MaterialFunctionID : u16;
enum class MaterialOpCode : u8;
struct opt_bool;
struct uv;
struct MaterialCondition;
struct MATR;
struct MATC;
using MaterialConditionParam = std::variant<std::string, bool, double>;

// Implementations
struct opt_bool {
  constexpr opt_bool() : value_{false}, hasValue_{false} {}
  constexpr explicit opt_bool(bool value) : value_{value}, hasValue_{true} {}

  _NODISCARD constexpr bool has_value() const { return hasValue_; }

  _NODISCARD constexpr bool value() const { return value_; }

  _NODISCARD constexpr bool value_or(bool other) const {
    return hasValue_ ? value_ : other;
  }

  constexpr explicit operator bool() const noexcept {
    return hasValue_ ? value_ : false;
  }

  constexpr bool operator==(const opt_bool& rhs) const {
    return hasValue_ == rhs.hasValue_ && value_ == rhs.value_;
  }

  constexpr bool operator==(const bool rhs) const {
    return hasValue_ && value_ == rhs;
  }

 private:
  bool value_ BITFIELD(1);
  bool hasValue_ BITFIELD(1);
};

struct uv {
  explicit uv(const half& scaleX, const half& scaleY, const half& offsetX,
              const half& offsetY)
      : data_(scaleX, scaleY, offsetX, offsetY) {}

  explicit uv(const array<half, 2>& scale, const array<half, 2>& offset)
      : data_(scale[0], scale[1], offset[0], offset[1]) {}

  _NODISCARD fn scale() const -> RE::NiPoint2 {
    return RE::NiPoint2(half::ToFloat32Fast(data_[0]),
                        half::ToFloat32Fast(data_[1]));
  }

  _NODISCARD fn offset() const -> RE::NiPoint2 {
    return RE::NiPoint2(half::ToFloat32Fast(data_[2]),
                        half::ToFloat32Fast(data_[3]));
  }

 private:
  half data_[4];
};

enum class ColorBlendMode : u8 {
  Normal,
  Add,
  Multiply,
  Screen,
  Overlay,
  Darken,
  Lighten,
  ColorDodge,
  ColorBurn,
  HardLight,
  SoftLight,
};

enum class MaterialFlags : u32 {
  Decal = ENUMI(0),
  TwoSided = ENUMI(1),
  EnvMapEnabled = ENUMI(2),
  SpecularEnabled = ENUMI(3),
  ReceiveShadows = ENUMI(4),
  CastShadows = ENUMI(5),
  Facegen = ENUMI(6),
  Hair = ENUMI(7),
  DecalNoFade = ENUMI(8),
  NonOccluder = ENUMI(9),
  Refraction = ENUMI(10),
  RefractionFalloff = ENUMI(11),
  GrayscaleToPaletteColor = ENUMI(12),
  DepthWrite = ENUMI(13),
  SubsurfaceLighting = ENUMI(14),
  EnvMapEye = ENUMI(15),
  EmitEnabled = ENUMI(16),
  Pbr = ENUMI(17),
  BackLighting = ENUMI(18),
  HideSecret = ENUMI(19),
  DissolveFade = ENUMI(20),
  AssumeShadowMask = ENUMI(21),
  GlowMapEnabled = ENUMI(22),
  EnableEditorAlphaThreshold = ENUMI(23),
  Translucency = ENUMI(24),
  TranslucencyThickObject = ENUMI(25),
  TranslucencyMixAlbedoWithSubsurfaceColor = ENUMI(26),
  RimLighting = ENUMI(27),
  AlphaBlend = ENUMI(28),
  AlphaTest = ENUMI(29),
  DepthBias = ENUMI(30),
  CustomPorosity = ENUMI(31),
  AnisoLighting = ENUMI(32),
  ModelSpaceNormals = ENUMI(33),
  ExternalEmit = ENUMI(34),
  UseAdaptiveEmissive = ENUMI(35),
  DepthTest = ENUMI(36),
  Invisible = ENUMI(37),

  MAX = ENUMI(38),
};

#define COPY_FUNC(_COPY_FUNC) \
  ##_COPY_FUNC = static_cast<u16>(RE::FUNCTION_DATA::FunctionID::k##_COPY_FUNC)
#define VANILLA_MAX() \
  VanillaMax = static_cast<u16>(RE::FUNCTION_DATA::FunctionID::kTotal)

enum class MaterialFunctionID : u16 {
  // vanilla
  COPY_FUNC(GetWantBlocking),
  COPY_FUNC(GetDistance),
  COPY_FUNC(GetLocked),
  COPY_FUNC(GetPos),
  COPY_FUNC(GetAngle),
  COPY_FUNC(GetStartingPos),
  COPY_FUNC(GetStartingAngle),
  COPY_FUNC(GetSecondsPassed),
  COPY_FUNC(GetActorValue),
  COPY_FUNC(GetAV),
  COPY_FUNC(GetCurrentTime),
  COPY_FUNC(GetScale),
  COPY_FUNC(IsMoving),
  COPY_FUNC(IsTurning),
  COPY_FUNC(GetLineOfSight),
  COPY_FUNC(GetLOS),
  COPY_FUNC(GetButtonPressed),
  COPY_FUNC(GetInSameCell),
  COPY_FUNC(GetDisabled),
  COPY_FUNC(MenuMode),
  COPY_FUNC(GetDisease),
  COPY_FUNC(GetClothingValue),
  COPY_FUNC(SameFaction),
  COPY_FUNC(SameRace),
  COPY_FUNC(SameSex),
  COPY_FUNC(GetDetected),
  COPY_FUNC(GetDead),
  COPY_FUNC(GetItemCount),
  COPY_FUNC(GetGold),
  COPY_FUNC(GetSleeping),
  COPY_FUNC(GetTalkedToPC),
  COPY_FUNC(GetScriptVariable),
  COPY_FUNC(GetQuestRunning),
  COPY_FUNC(GetQR),
  COPY_FUNC(GetStage),
  COPY_FUNC(GetStageDone),
  COPY_FUNC(GetFactionRankDifference),
  COPY_FUNC(GetAlarmed),
  COPY_FUNC(IsRaining),
  COPY_FUNC(GetAttacked),
  COPY_FUNC(GetIsCreature),
  COPY_FUNC(GetLockLevel),
  COPY_FUNC(GetShouldAttack),
  COPY_FUNC(GetInCell),
  COPY_FUNC(GetIsClass),
  COPY_FUNC(GetIsRace),
  COPY_FUNC(GetIsSex),
  COPY_FUNC(GetInFaction),
  COPY_FUNC(GetIsID),
  COPY_FUNC(GetFactionRank),
  COPY_FUNC(GetGlobalValue),
  COPY_FUNC(IsSnowing),
  COPY_FUNC(GetRandomPercent),
  COPY_FUNC(GetQuestVariable),
  COPY_FUNC(GetLevel),
  COPY_FUNC(IsRotating),
  COPY_FUNC(GetLeveledEncounterValue),
  COPY_FUNC(GetDeadCount),
  COPY_FUNC(GetIsAlerted),
  COPY_FUNC(EvaluatePackage),
  COPY_FUNC(GetPlayerControlsDisabled),
  COPY_FUNC(GetHeadingAngle),
  COPY_FUNC(IsWeaponMagicOut),
  COPY_FUNC(IsTorchOut),
  COPY_FUNC(IsShieldOut),
  COPY_FUNC(IsActionRef),
  COPY_FUNC(IsFacingUp),
  COPY_FUNC(GetKnockedState),
  COPY_FUNC(GetWeaponAnimType),
  COPY_FUNC(GetCurrentAIPackage),
  COPY_FUNC(IsWaiting),
  COPY_FUNC(IsIdlePlaying),
  COPY_FUNC(IsIntimidatedByPlayer),
  COPY_FUNC(IsPlayerInRegion),
  COPY_FUNC(GetActorAggroRadiusViolated),
  COPY_FUNC(GetCrimeKnown),
  COPY_FUNC(GetCrime),
  COPY_FUNC(IsGreetingPlayer),
  COPY_FUNC(IsGuard),
  COPY_FUNC(GetStaminaPercentage),
  COPY_FUNC(GetPCIsClass),
  COPY_FUNC(GetPCIsRace),
  COPY_FUNC(GetPCIsSex),
  COPY_FUNC(GetPCInFaction),
  COPY_FUNC(SameFactionAsPC),
  COPY_FUNC(SameRaceAsPC),
  COPY_FUNC(SameSexAsPC),
  COPY_FUNC(GetIsReference),
  COPY_FUNC(IsTalking),
  COPY_FUNC(GetWalkSpeed),
  COPY_FUNC(GetCurrentAIProcedure),
  COPY_FUNC(IsTrespassing),
  COPY_FUNC(IsInMyOwnedCell),
  COPY_FUNC(GetWindSpeed),
  COPY_FUNC(GetCurrentWeatherPercent),
  COPY_FUNC(GetIsCurrentWeather),
  COPY_FUNC(IsContinuingPackagePCNear),
  COPY_FUNC(GetIsCrimeFaction),
  COPY_FUNC(CanHaveFlames),
  COPY_FUNC(HasFlames),
  COPY_FUNC(GetOpenState),
  COPY_FUNC(GetSitting),
  COPY_FUNC(GetIsCurrentPackage),
  COPY_FUNC(IsCurrentFurnitureRef),
  COPY_FUNC(IsCurrentFurnitureObj),
  COPY_FUNC(GetFactionReaction),
  COPY_FUNC(GetDayOfWeek),
  COPY_FUNC(GetTalkedToPCParam),
  COPY_FUNC(IsPCSleeping),
  COPY_FUNC(IsPCAMurderer),
  COPY_FUNC(HasSameEditorLocAsRef),
  COPY_FUNC(HasSameEditorLocAsRefAlias),
  COPY_FUNC(GetEquipped),
  COPY_FUNC(IsSwimming),
  COPY_FUNC(ScriptEffectElapsedSeconds),
  COPY_FUNC(GetPCSleepHours),
  COPY_FUNC(GetAmountSoldStolen),
  COPY_FUNC(GetIgnoreCrime),
  COPY_FUNC(GetPCExpelled),
  COPY_FUNC(GetPCFactionMurder),
  COPY_FUNC(GetPCEnemyofFaction),
  COPY_FUNC(GetPCFactionAttack),
  COPY_FUNC(GetDestroyed),
  COPY_FUNC(GetActionRef),
  COPY_FUNC(GetSelf),
  COPY_FUNC(GetContainer),
  COPY_FUNC(GetForceRun),
  COPY_FUNC(GetForceSneak),
  COPY_FUNC(HasMagicEffect),
  COPY_FUNC(GetDefaultOpen),
  COPY_FUNC(IsSpellTarget),
  COPY_FUNC(GetVATSMode),
  COPY_FUNC(GetPersuasionNumber),
  COPY_FUNC(GetCannibal),
  COPY_FUNC(GetIsClassDefault),
  COPY_FUNC(GetClassDefaultMatch),
  COPY_FUNC(GetInCellParam),
  COPY_FUNC(GetCombatTarget),
  COPY_FUNC(GetPackageTarget),
  COPY_FUNC(GetVatsTargetHeight),
  COPY_FUNC(GetIsGhost),
  COPY_FUNC(GetUnconscious),
  COPY_FUNC(GetRestrained),
  COPY_FUNC(GetIsUsedItem),
  COPY_FUNC(IsScenePlaying),
  COPY_FUNC(IsInDialogueWithPlayer),
  COPY_FUNC(GetLocationCleared),
  COPY_FUNC(GetIsPlayableRace),
  COPY_FUNC(GetOffersServicesNow),
  COPY_FUNC(GetGameSetting),
  COPY_FUNC(HasAssociationType),
  COPY_FUNC(Family),
  COPY_FUNC(IsParent),
  COPY_FUNC(IsWarningAbout),
  COPY_FUNC(IsWeaponOut),
  COPY_FUNC(HasSpell),
  COPY_FUNC(IsTimePassing),
  COPY_FUNC(IsPleasant),
  COPY_FUNC(IsCloudy),
  COPY_FUNC(IsSmallBump),
  COPY_FUNC(GetBaseActorValue),
  COPY_FUNC(IsOwner),
  COPY_FUNC(IsCellOwner),
  COPY_FUNC(IsHorseStolen),
  COPY_FUNC(IsLeftUp),
  COPY_FUNC(IsSneaking),
  COPY_FUNC(IsRunning),
  COPY_FUNC(IsInCombat),
  COPY_FUNC(IsAnimPlaying),
  COPY_FUNC(IsInInterior),
  COPY_FUNC(IsActorsAIOff),
  COPY_FUNC(IsWaterObject),
  COPY_FUNC(IsActorUsingATorch),
  COPY_FUNC(GetPCMiscStat),
  COPY_FUNC(GetPairedAnimation),
  COPY_FUNC(IsActorAVictim),
  COPY_FUNC(GetTotalPersuasionNumber),
  COPY_FUNC(GetNoRumors),
  COPY_FUNC(GetCombatState),
  COPY_FUNC(GetWithinPackageLocation),
  COPY_FUNC(IsRidingMount),
  COPY_FUNC(IsFleeing),
  COPY_FUNC(IsInDangerousWater),
  COPY_FUNC(GetIgnoreFriendlyHits),
  COPY_FUNC(IsPlayersLastRiddenMount),
  COPY_FUNC(IsActor),
  COPY_FUNC(IsEssential),
  COPY_FUNC(IsPlayerMovingIntoNewSpace),
  COPY_FUNC(GetInCurrentLoc),
  COPY_FUNC(GetInCurrentLocAlias),
  COPY_FUNC(GetTimeDead),
  COPY_FUNC(HasLinkedRef),
  COPY_FUNC(IsChild),
  COPY_FUNC(GetStolenItemValueNoCrime),
  COPY_FUNC(GetLastPlayerAction),
  COPY_FUNC(IsPlayerActionActive),
  COPY_FUNC(IsTalkingActivatorActor),
  COPY_FUNC(IsInList),
  COPY_FUNC(GetStolenItemValue),
  COPY_FUNC(GetCrimeGoldViolent),
  COPY_FUNC(GetCrimeGoldNonviolent),
  COPY_FUNC(HasShout),
  COPY_FUNC(GetObjectiveFailed),
  COPY_FUNC(GetHitLocation),
  COPY_FUNC(IsPC1stPerson),
  COPY_FUNC(GetCauseofDeath),
  COPY_FUNC(IsLimbGone),
  COPY_FUNC(IsWeaponInList),
  COPY_FUNC(IsBribedbyPlayer),
  COPY_FUNC(GetRelationshipRank),
  COPY_FUNC(GetVATSValue),
  COPY_FUNC(IsKiller),
  COPY_FUNC(IsKillerObject),
  COPY_FUNC(GetFactionCombatReaction),
  COPY_FUNC(EvaluateSpellConditions),
  COPY_FUNC(Exists),
  COPY_FUNC(GetGroupMemberCount),
  COPY_FUNC(GetGroupTargetCount),
  COPY_FUNC(GetObjectiveCompleted),
  COPY_FUNC(GetObjectiveDisplayed),
  COPY_FUNC(GetIsFormType),
  COPY_FUNC(GetIsVoiceType),
  COPY_FUNC(GetPlantedExplosive),
  COPY_FUNC(IsScenePackageRunning),
  COPY_FUNC(GetHealthPercentage),
  COPY_FUNC(GetIsObjectType),
  COPY_FUNC(GetDialogueEmotion),
  COPY_FUNC(GetDialogueEmotionValue),
  COPY_FUNC(GetIsCreatureType),
  COPY_FUNC(GetInCurrentLocFormList),
  COPY_FUNC(GetInZone),
  COPY_FUNC(GetVelocity),
  COPY_FUNC(GetGraphVariableFloat),
  COPY_FUNC(HasPerk),
  COPY_FUNC(GetFactionRelation),
  COPY_FUNC(IsLastIdlePlayed),
  COPY_FUNC(GetPlayerTeammate),
  COPY_FUNC(GetPlayerTeammateCount),
  COPY_FUNC(GetActorCrimePlayerEnemy),
  COPY_FUNC(GetCrimeGold),
  COPY_FUNC(GetPlayerGrabbedRef),
  COPY_FUNC(IsPlayerGrabbedRef),
  COPY_FUNC(GetKeywordItemCount),
  COPY_FUNC(GetBroadcastState),
  COPY_FUNC(GetDestructionStage),
  COPY_FUNC(GetIsAlignment),
  COPY_FUNC(IsProtected),
  COPY_FUNC(GetThreatRatio),
  COPY_FUNC(GetIsUsedItemEquipType),
  COPY_FUNC(GetPlayerName),
  COPY_FUNC(MatchRace),
  COPY_FUNC(IsCarryable),
  COPY_FUNC(GetConcussed),
  COPY_FUNC(GetPermanentActorValue),
  COPY_FUNC(GetKillingBlowLimb),
  COPY_FUNC(CanPayCrimeGold),
  COPY_FUNC(GetDaysInJail),
  COPY_FUNC(GetLastHitCritical),
  COPY_FUNC(IsCombatTarget),
  COPY_FUNC(GetIsLockBroken),
  COPY_FUNC(IsInCriticalStage),
  COPY_FUNC(GetXPForNextLevel),
  COPY_FUNC(GetInfamy),
  COPY_FUNC(GetInfamyViolent),
  COPY_FUNC(GetInfamyNonViolent),
  COPY_FUNC(IsGoreDisabled),
  COPY_FUNC(IsSceneActionComplete),
  COPY_FUNC(GetSpellUsageNum),
  COPY_FUNC(HasLoaded3D),
  COPY_FUNC(IsImageSpaceActive),
  COPY_FUNC(HasKeyword),
  COPY_FUNC(HasRefType),
  COPY_FUNC(LocationHasKeyword),
  COPY_FUNC(LocationHasRefType),
  COPY_FUNC(GetIsEditorLocation),
  COPY_FUNC(GetIsAliasRef),
  COPY_FUNC(GetIsEditorLocAlias),
  COPY_FUNC(IsSprinting),
  COPY_FUNC(IsBlocking),
  COPY_FUNC(HasEquippedSpell),
  COPY_FUNC(GetCurrentCastingType),
  COPY_FUNC(GetCasting),
  COPY_FUNC(GetCurrentDeliveryType),
  COPY_FUNC(GetAttackState),
  COPY_FUNC(GetAliasedRef),
  COPY_FUNC(GetEventData),
  COPY_FUNC(IsCloserToAThanB),
  COPY_FUNC(GetEquippedShout),
  COPY_FUNC(IsBleedingOut),
  COPY_FUNC(GetRelativeAngle),
  COPY_FUNC(GetMovementDirection),
  COPY_FUNC(IsInScene),
  COPY_FUNC(GetRefTypeDeadCount),
  COPY_FUNC(GetRefTypeAliveCount),
  COPY_FUNC(GetIsFlying),
  COPY_FUNC(IsCurrentSpell),
  COPY_FUNC(SpellHasKeyword),
  COPY_FUNC(GetEquippedItemType),
  COPY_FUNC(GetLocationAliasCleared),
  COPY_FUNC(GetLocAliasRefTypeDeadCount),
  COPY_FUNC(GetLocAliasRefTypeAliveCount),
  COPY_FUNC(IsWardState),
  COPY_FUNC(IsInSameCurrentLocAsRef),
  COPY_FUNC(IsInSameCurrentLocAsRefAlias),
  COPY_FUNC(LocAliasIsLocation),
  COPY_FUNC(GetKeywordDataForLocation),
  COPY_FUNC(GetKeywordDataForAlias),
  COPY_FUNC(LocAliasHasKeyword),
  COPY_FUNC(IsNullPackageData),
  COPY_FUNC(GetNumericPackageData),
  COPY_FUNC(IsFurnitureAnimType),
  COPY_FUNC(IsFurnitureEntryType),
  COPY_FUNC(GetHighestRelationshipRank),
  COPY_FUNC(GetLowestRelationshipRank),
  COPY_FUNC(HasAssociationTypeAny),
  COPY_FUNC(HasFamilyRelationshipAny),
  COPY_FUNC(GetPathingTargetOffset),
  COPY_FUNC(GetPathingTargetAngleOffset),
  COPY_FUNC(GetPathingTargetSpeed),
  COPY_FUNC(GetPathingTargetSpeedAngle),
  COPY_FUNC(GetMovementSpeed),
  COPY_FUNC(GetInContainer),
  COPY_FUNC(IsLocationLoaded),
  COPY_FUNC(IsLocAliasLoaded),
  COPY_FUNC(IsDualCasting),
  COPY_FUNC(GetVMQuestVariable),
  COPY_FUNC(GetVMScriptVariable),
  COPY_FUNC(IsEnteringInteractionQuick),
  COPY_FUNC(IsCasting),
  COPY_FUNC(GetFlyingState),
  COPY_FUNC(IsInFavorState),
  COPY_FUNC(HasTwoHandedWeaponEquipped),
  COPY_FUNC(IsExitingInstant),
  COPY_FUNC(IsInFriendStateWithPlayer),
  COPY_FUNC(GetActorValuePercent),
  COPY_FUNC(IsUnique),
  COPY_FUNC(IsInFurnitureState),
  COPY_FUNC(GetIsInjured),
  COPY_FUNC(GetIsCrashLandRequest),
  COPY_FUNC(GetIsHastyLandRequest),
  COPY_FUNC(IsLinkedTo),
  COPY_FUNC(GetKeywordDataForCurrentLocation),
  COPY_FUNC(GetInSharedCrimeFaction),
  COPY_FUNC(GetBribeAmount),
  COPY_FUNC(GetBribeSuccess),
  COPY_FUNC(GetIntimidateSuccess),
  COPY_FUNC(GetArrestedState),
  COPY_FUNC(GetReceivedGiftValue),
  COPY_FUNC(GetGiftGivenValue),
  COPY_FUNC(GetReplacedItemType),
  COPY_FUNC(IsAttacking),
  COPY_FUNC(IsPowerAttacking),
  COPY_FUNC(IsLastHostileActor),
  COPY_FUNC(GetGraphVariableInt),
  COPY_FUNC(GetCurrentShoutVariation),
  COPY_FUNC(GetActivationHeight),
  COPY_FUNC(WornHasKeyword),
  COPY_FUNC(GetPathingCurrentSpeed),
  COPY_FUNC(GetPathingCurrentSpeedAngle),
  COPY_FUNC(IsAttackType),
  COPY_FUNC(IsAllowedToFly),
  COPY_FUNC(HasMagicEffectKeyword),
  COPY_FUNC(IsCommandedActor),
  COPY_FUNC(IsStaggered),
  COPY_FUNC(IsRecoiling),
  COPY_FUNC(IsExitingInteractionQuick),
  COPY_FUNC(IsPathing),
  COPY_FUNC(GetShouldHelp),
  COPY_FUNC(HasBoundWeaponEquipped),
  COPY_FUNC(GetCombatTargetHasKeyword),
  COPY_FUNC(GetCombatGroupMemberCount),
  COPY_FUNC(IsIgnoringCombat),
  COPY_FUNC(GetLightLevel),
  COPY_FUNC(SpellHasCastingPerk),
  COPY_FUNC(IsBeingRidden),
  COPY_FUNC(IsUndead),
  COPY_FUNC(GetRealHoursPassed),
  COPY_FUNC(IsUnlockedDoor),
  COPY_FUNC(IsHostileToActor),
  COPY_FUNC(GetTargetHeight),
  COPY_FUNC(IsPoison),
  COPY_FUNC(WornApparelHasKeywordCount),
  COPY_FUNC(GetItemHealthPercent),
  COPY_FUNC(EffectWasDualCast),
  COPY_FUNC(GetKnockStateEnum),
  COPY_FUNC(DoesNotExist),
  COPY_FUNC(IsOnFlyingMount),
  COPY_FUNC(CanFlyHere),
  COPY_FUNC(IsFlyingMountPatrolQueued),
  COPY_FUNC(IsFlyingMountFastTravelling),
  COPY_FUNC(IsOverEncumbered),
  COPY_FUNC(GetActorWarmth),

  VANILLA_MAX(),
  // custom
  GetRaceMenuMorph,

  Total,
};

struct MaterialCondition {
  MaterialFunctionID function;
  RE::CONDITION_ITEM_DATA::OpCode op;
  bool negate;
  std::vector<MaterialConditionParam> params;
};

struct MATR {
  FileID id;
  opt_bitset<37> flags;

  FileID inherits{0};

  // textures
  FileID diffuseMap{0};
  FileID normalMap{0};
  FileID smoothSpecMap{0};
  FileID grayscaleMap{0};
  FileID glowMap{0};
  FileID wrinkleMap{0};
  FileID specularMap{0};
  FileID lightingMap{0};
  FileID flowMap{0};
  FileID distanceFieldAlphaMap{0};
  FileID envMap{0};
  FileID innerLayerMap{0};
  FileID displacementMap{0};
  FileID baseMap{0};
  FileID envMapMask{0};
  FileID faceTintMap{0};
  FileID detailMap{0};
  FileID subsurfaceMap{0};
  FileID parallaxMap{0};
  FileID rmaosMap{0};
  FileID coatMap{0};
  FileID coatNormalMap{0};
  FileID fuzzMap{0};
  FileID emissionMap{0};
  FileID colorBlendMap{0};

  opt<u8> clamp;
  opt<uv> uv;
  opt<half> transparency;
  opt<u8> sourceBlendMode;
  opt<u8> destinationBlendMode;
  opt<u8> alphaTestThreshold;
  opt<u8> maskWrites;
  opt<half> refractionPower;
  opt<half> envMapMaskScale;
  opt<half> rimPower;
  opt<half> backLightPower;
  opt<half> specularPower;
  opt<half> subsurfaceLightingRolloff;
  opt<array<u8, 3>> specularColor;
  opt<half> specularMult;
  opt<half> smoothness;
  opt<half> fresnelPower;
  opt<array<u8, 3>> emitColor;
  opt<half> emitMult;

  // custom fields
  opt<ColorBlendMode> colorBlendMode;
  opt<array<u8, 4>> colorChannelR;
  opt<array<u8, 4>> colorChannelG;
  opt<array<u8, 4>> colorChannelB;

  bool operator==(const MATR& rhs) const {
    return rhs.id == id;
  }
};

struct MATC {
  RE::FormID form;
  string name;
  std::vector<std::tuple<std::string, FileID>> applies;
  std::vector<std::string> keywords;
  std::vector<MaterialCondition> conditions;
  u8 layer;
  u8 gender;
  bool isHidden = false;
  bool modifyName = true;

  _NODISCARD fn find_material(const std::string& shape) const {
    for (const auto [key, value] : applies) {
      if (key == shape) return std::optional(value);
    }
    return std::nullopt;
  }

  bool operator==(const MATC& rhs) const {
    return rhs.form == form && rhs.name == rhs.name;
  }
};