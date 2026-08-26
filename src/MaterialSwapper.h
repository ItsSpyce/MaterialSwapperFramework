#pragma once

#include "Types.h"

namespace MaterialSwapper {
/**
 * Reads the material configurations from disk.
 */
void ReadMaterialConfigurations();
void VisitMaterialFiles(RE::FormID formID, const Visitor<MATC>& visitor);
_NODISCARD std::optional<MATR> GetMaterialRecord(FileID fileID);
_NODISCARD std::optional<MATC> GetMaterialConfig(RE::FormID formID,
                                                 const std::string& name);
_NODISCARD std::optional<MATC> GetDefaultMaterialConfig(RE::FormID formID);
_NODISCARD bool HasMaterialConfigs(RE::FormID formID);

// Universal
void ResetAll();
void VisitApplicableMaterials(const RE::TESForm* form,
                              const Visitor<MATC>& visitor);

// Armors
void ResetEquippedArmors(RE::Actor* actor);
void ResetEquippedArmor(RE::Actor* actor, RE::BipedObjectSlot slot);
void ApplyArmorMaterial(RE::Actor* actor, RE::BipedObjectSlot slot,
                        const MATC& config);
void LoadArmorMaterials(RE::Actor* actor);
void LoadArmorMaterials(RE::Actor* actor, RE::BipedObjectSlot slot);
void VisitAppliedArmorMaterials(RE::Actor* actor, RE::InventoryEntryData* data,
                                const Visitor<MATC>& visitor);
void VisitAppliedArmorMaterials(RE::Actor* actor, RE::BipedObjectSlot slot,
                                const Visitor<MATC>& visitor);
result<RE::NiNode*> RenderArmorMaterials(RE::Actor* actor, RE::BipedObjectSlot slot);

// Weapons
void ResetEquippedWeapons(RE::Actor* actor);
void ResetEquippedWeapon(RE::Actor* actor, bool left);
void ApplyWeaponMaterial(RE::Actor* actor, bool left, const MATC& config);
void LoadWeaponMaterials(RE::Actor* actor);
void LoadWeaponMaterials(RE::Actor* actor, bool left);
void VisitAppliedWeaponMaterials(RE::Actor* actor, bool left,
                                 const Visitor<MATC>& visitor);
}  // namespace MaterialSwapper