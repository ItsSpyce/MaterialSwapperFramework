#pragma once

#include "Events/EventListener.h"
#include "Helpers/RaceMenuHelpers.h"
#include "MaterialSwapper.h"
#include "MeshBuilder.h"
#include "RE/Offset.h"
#include "RE/BipedSkinContext.h"

extern EventSource<PlayerViewChangeEvent> g_playerViewChangeSource;

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
    auto* result =
        func(_this, loaded, skeleton, bipedSlot, flag, a6, textureOverride);
    auto* actor =
        skeleton ? skyrim_cast<RE::Actor*>(skeleton->GetUserData()) : nullptr;
    if (actor) {
      MaterialSwapper::VisitAppliedArmorMaterials(
          actor, (RE::BipedObjectSlot)bipedSlot, [&result](const MATC& matc) {
            MeshBuilder::ApplyMaterialToMesh(result->AsNode(), matc);
            return RE::BSVisit::BSVisitControl::kContinue;
          });
    }
    return result;
  }

  static inline REL::Relocation<decltype(&thunk)> func{
      RE::Offset::BipedAnim::AttachSkinnedObject};
};

struct Actor_CreateWeaponNodes {
  static void __fastcall thunk(RE::TESObjectREFR* actor, RE::TESForm* weap,
                               bool left) {
    func(actor, weap, left);
    // Factories::WeaponFactory::GetSingleton()->ApplySavedMaterial(actor->As<RE::Actor>(),
    // left);
  }

  static inline REL::Relocation<decltype(&thunk)> func{
      RE::Offset::Actor::CreateWeaponNodes};
};

struct Main_Update {
  static void thunk() {
    func();
    auto* main = RE::Main::GetSingleton();
    if (!main->GetRuntimeData().gameActive) {
      // do something later maybe
      _TRACE("Quit game called");
      return;
    }
    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player) {
      _TRACE("No player found");
      return;
    }
    if (auto* currentCell = player->GetParentCell()) {
      if (playerCurrentCell != currentCell->GetFormID()) {
        const PlayerCellChangeEvent cellEvent{
            .isExterior = currentCell->IsExteriorCell(),
            .isChangedInOut = isExteriorCell != currentCell->IsExteriorCell(),
            .oldCell = playerCurrentCell,
            .newCell = currentCell->GetFormID(),
        };
        g_cellChangeSource.Dispatch(cellEvent);
        _TRACE(
            "PlayerCellChangeEvent dispatched: isExterior={}, "
            "isChangedInOut={}",
            cellEvent.isExterior, cellEvent.isChangedInOut);
      }
      playerCurrentCell = currentCell->GetFormID();
      isExteriorCell = currentCell->IsExteriorCell();
    }
    if (auto* sky = RE::Sky::GetSingleton()) {
      auto* weather = sky->overrideWeather  ? sky->overrideWeather
                      : sky->currentWeather ? sky->currentWeather
                                            : sky->defaultWeather;
      if (weather && weather->data.flags.underlying() != currentWeather) {
        g_weatherChangeSource.Dispatch(WeatherChangeEvent{});
        currentWeather = weather->data.flags.underlying();
      }
    }
    const FrameEvent frameEvent{
        .gamePaused = main ? main->GetRuntimeData().freezeTime : false,
    };
    g_frameEventSource.Dispatch(frameEvent);
  }

  static inline REL::Relocation<decltype(&thunk)> func;
  static inline REL::Relocation rel{RE::Offset::Main::Update};
  static inline REL::Relocation offset{RE::Offset::Main::UpdateOffset};
  static inline RE::FormID playerCurrentCell;
  static inline u8 currentWeather;
  static inline bool isExteriorCell;
};

struct PlayerCamera_SwitchPOV {
  static void thunk(RE::PlayerCamera* _this, void* a1) {
    func(_this, a1);
    const auto thirdPerson =
        RE::PlayerCamera::GetSingleton()->IsInThirdPerson();
    if (thirdPerson != wasLastChangeThirdPerson) {
      wasLastChangeThirdPerson = thirdPerson;
      const PlayerViewChangeEvent event{
          .thirdPerson = thirdPerson,
      };
      g_playerViewChangeSource.Dispatch(event);
    }
  }

  static inline REL::Relocation<decltype(&thunk)> func;
  static inline REL::Relocation rel{RE::Offset::PlayerCamera::UpdatePOV};
  static inline REL::Relocation offset{
      RE::Offset::PlayerCamera::UpdatePOVOffset};
  static inline bool wasLastChangeThirdPerson{false};
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
              if (ranges::contains(config.keywords,
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
  stl::write_thunk_call<Main_Update>();
  stl::write_thunk_call<PlayerCamera_SwitchPOV>();
}
}  // namespace Hooks