#pragma once

#include "EditorIDCache.h"
#include "Events/EventListener.h"
#include "RE/Offset.h"

extern EventSource<FrameEvent> g_frameEventSource;
extern EventSource<ArmorAttachEvent> g_armorAttachSource;
extern EventSource<PlayerCellChangeEvent> g_cellChangeSource;
extern EventSource<WeatherChangeEvent> g_weatherChangeSource;

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

struct Actor_AttachArmor {
  static RE::NiAVObject* __fastcall thunk(void* _this, RE::NiNode* armor,
                                          RE::NiNode* skeleton, i32 bipedSlot,
                                          void* a3, void* a4, void* a5,
                                          void* a6, char a7, i32 a8, void* a9) {
    ArmorAttachEvent event{
        .actor = skeleton ? skyrim_cast<RE::Actor*>(skeleton->GetUserData())
                          : nullptr,
        .armor = armor,
        .bipedSlot = bipedSlot,
    };
    auto* ref =
        func(_this, armor, skeleton, bipedSlot, a3, a4, a5, a6, a7, a8, a9);
    if (ref) {
      event.attachedAt = ref;
      event.hasAttached = true;
    }
    _TRACE(
        "Dispatching ArmorAttachEvent for actor: {0:X}, armor: {1}, bipedSlot: "
        "{2}, hasAttached: {3}",
        event.actor ? event.actor->GetFormID() : 0,
        armor ? armor->name.c_str() : "null", event.bipedSlot,
        event.hasAttached);
    g_armorAttachSource.Dispatch(event);
    return ref;
  }

  static inline REL::Relocation<decltype(&thunk)> func{
      RE::Offset::Actor::AttachArmor};
};

struct Actor_CreateWeaponNodes {
  static void __fastcall thunk(RE::TESObjectREFR* actor, RE::TESForm* weap, bool left) {
    func(actor, weap, left);
    Factories::WeaponFactory::GetSingleton()->ApplySavedMaterial(actor->As<RE::Actor>(), left);
  }

  static inline REL::Relocation<decltype(&thunk)> func{
    RE::Offset::Actor::CreateWeaponNodes
  };
};

struct Main_Update {
  static void thunk() {
    func();
    auto* main = RE::Main::GetSingleton();
    if (main->quitGame) {
      // do something later maybe
      return;
    }
    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player) {
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
        .gamePaused = main ? main->freezeTime : false,
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

inline void Install() noexcept {
  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  stl::write_detour<Actor_AttachArmor>();
  stl::write_detour<Actor_CreateWeaponNodes>();
  DetourTransactionCommit();

  stl::write_vfunc<RE::TESForm, TESForm_SetFormEditorID>();
  stl::write_thunk_call<Main_Update>();
}
}  // namespace Hooks