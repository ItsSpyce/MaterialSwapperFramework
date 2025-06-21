#pragma once

namespace MaterialPapyrus {
bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);

int GetVersion(RE::StaticFunctionTag*);

template<typename T>
T GetMaterials(RE::StaticFunctionTag*);

template<typename T>
T GetMaterialsForArmor(RE::StaticFunctionTag*, RE::TESObjectARMO* armo);

template <typename T>
T GetMaterialsForSlot(RE::StaticFunctionTag*, RE::TESObjectARMO* armo,
                           int slot);

template <typename T>
T GetMaterialsForActor(RE::StaticFunctionTag*, RE::Actor* actor);

template <typename T>
T GetCurrentMaterialForArmor(RE::StaticFunctionTag*, RE::TESObjectREFR* refr, RE::TESObjectARMO* armo);

template <typename T>
T GetCurrentMaterialForSlot(RE::StaticFunctionTag*,
                                 RE::TESObjectREFR* refr, int slot);

template <typename T>
T GetCurrentMaterialForActor(RE::StaticFunctionTag*, RE::Actor* actor);

template <typename T>
T ApplyMaterialToArmor(RE::StaticFunctionTag*, RE::TESObjectREFR* refr,
                       RE::TESObjectARMO* armo, const RE::BSFixedString& materialName);

template <typename T>
T ApplyMaterialToSlot(RE::StaticFunctionTag*, RE::TESObjectREFR* refr,
                           int slot, const RE::BSFixedString& materialName);

template <typename T>
T ApplyMaterialToActor(RE::StaticFunctionTag*, RE::Actor* actor,
                       const RE::BSFixedString& materialName);

template <typename T>
T ResetMaterialForArmor(RE::StaticFunctionTag*, RE::TESObjectREFR* refr,
                        RE::TESObjectARMO* armo);

template <typename T>
T ResetMaterialForSlot(RE::StaticFunctionTag*, RE::TESObjectREFR* refr,
                       int slot);

template <typename T>
T ResetMaterialForActor(RE::StaticFunctionTag*, RE::Actor* actor);
}