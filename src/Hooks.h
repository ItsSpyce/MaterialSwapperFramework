#pragma once

#include "Events/EventListener.h"
#include "Helpers/RaceMenuHelpers.h"
#include "MaterialSwapper.h"
#include "MeshBuilder.h"
#include "RE/Offset.h"

namespace Hooks {
struct TESForm_SetFormEditorID {
  static bool thunk(RE::TESForm* form, const char* editorID) {
    if (std::strlen(editorID) > 0 && !form->IsDynamicForm()) {
      const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
      const RE::BSWriteLockGuard locker{lock};
      if (map) {
        map->emplace(editorID, form);
      }
      EditorIDCache::EditorID::GetSingleton()->CacheEditorID(form, editorID);
    }
    return func(form, editorID);
  }

  static inline REL::Relocation<decltype(&thunk)> func;
  static constexpr size_t idx{0x33};
};

struct BipedAnim_AttachSkinnedObject {
  static RE::NiAVObject* __fastcall thunk(RE::BipedAnim* _this,
                                          RE::NiNode* loaded,
                                          RE::NiNode* skeleton, i32 bipedSlot,
                                          bool flag, bool a6,
                                          RE::BSFaceGenModel* textureOverride) {
    auto* actor =
        skeleton ? skyrim_cast<RE::Actor*>(skeleton->GetUserData()) : nullptr;
    return MaterialSwapper::RenderArmorMaterials(actor,
                                                 (RE::BipedObjectSlot)bipedSlot)
        .match<RE::NiAVObject*>(
            [&](RE::NiNode* modified) {
              if (modified) return (RE::NiAVObject*)modified;
              return func(_this, loaded, skeleton, bipedSlot, flag, a6,
                          textureOverride);
            },
            [&](const std::string& err) {
              _ERROR("Failed to modify skinned object: {}", err);
              return func(_this, loaded, skeleton, bipedSlot, flag, a6,
                          textureOverride);
            });
  }

  static inline REL::Relocation<decltype(&thunk)> func{
      RE::Offset::BipedAnim::AttachSkinnedObject};
};

struct Actor_CreateWeaponNodes {
  static void __fastcall thunk(RE::TESObjectREFR* actor, RE::TESForm* weap,
                               bool left) {
    func(actor, weap, left);
  }

  static inline REL::Relocation<decltype(&thunk)> func{
      RE::Offset::Actor::CreateWeaponNodes};
};

struct InventoryUtils_WornHasKeyword {
  static bool __fastcall thunk(RE::InventoryEntryData* entryData,
                               RE::BGSKeyword* keyword) {
    if (auto* owner = entryData->GetOwner(); owner && owner->IsActor()) {
      auto* actor = owner->As<RE::Actor>();
      if (auto uid = Helpers::GetUniqueID(actor, entryData, false)) {
        bool didFind = false;
        MaterialSwapper::VisitAppliedArmorMaterials(
            actor, entryData, [&](const MATC& config) {
              if (std::ranges::contains(config.keywords,
                                        EditorIDCache::GetEditorID(keyword))) {
                didFind = true;
                return RE::BSVisit::BSVisitControl::kStop;
              }
              return RE::BSVisit::BSVisitControl::kContinue;
            });
        return didFind || func(entryData, keyword);
      }
    }
    return func(entryData, keyword);
  }

  static inline REL::Relocation<decltype(&thunk)> func{
      RE::RTTI_InventoryUtils____WornHasKeywordVisitor};
};

struct TESObjectREFR_WornHasKey {
  static bool thunk(RE::TESObjectREFR* _this, RE::BGSKeyword* keyword,
                    void* unused, double& result) {
    return func(_this, keyword, unused, result);
  }

  static inline REL::Relocation<decltype(&thunk)> func;
  static inline REL::Relocation rel{RE::Offset::TESObjectREFR::WornHasKeyword};
};

inline void Install() noexcept {
  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  stl::write_detour<BipedAnim_AttachSkinnedObject>();
  stl::write_detour<Actor_CreateWeaponNodes>();
  stl::write_detour<InventoryUtils_WornHasKeyword>();
  DetourTransactionCommit();

  stl::write_vfunc<RE::TESForm, TESForm_SetFormEditorID>();
}
}  // namespace Hooks