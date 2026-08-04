#pragma once

#include "Condition.h"
#include "Events/EventListener.h"
#include "Graphics/MaterialManager.h"
#include "IO/MaterialLoader.h"
#include "TaskManager.h"
#include "ThreadPool.h"

extern EventSource<ArmorAttachEvent> g_armorAttachSource;
extern unique_ptr<ThreadPool> g_updateThreads;
extern EventSource<FrameEvent> g_frameEventSource;
extern EventSource<PlayerCellChangeEvent> g_cellChangeSource;
extern EventSource<WeatherChangeEvent> g_weatherChangeSource;

namespace Conditions {
#define SINK(_T) \
 public          \
  RE::BSTEventSink<RE::_T>
#define SINK_HANDLE(_T)                  \
  RE::BSEventNotifyControl ProcessEvent( \
      const RE::_T* event, RE::BSTEventSource<RE::_T>* source) override
#define CONT return RE::BSEventNotifyControl::kContinue

class EvaluationContext : public Singleton<EvaluationContext>,
                          public EventListener<PlayerCellChangeEvent>,
                          public EventListener<ArmorAttachEvent>,
                          public EventListener<WeatherChangeEvent>,
                          /* SINK(TESActivateEvent),
                           SINK(TESActiveEffectApplyRemoveEvent),
                           SINK(TESActorLocationChangeEvent),
                           SINK(TESBookReadEvent),
                           SINK(TESCombatEvent),
                           SINK(TESContainerChangedEvent),
                           SINK(TESDeathEvent),
                           SINK(TESDestructionStageChangedEvent),
                           SINK(TESEnterBleedoutEvent),
                           SINK(TESEquipEvent),
                           SINK(TESFastTravelEndEvent),
                           SINK(TESFurnitureEvent),
                           SINK(TESGrabReleaseEvent),
                           SINK(TESHitEvent),
                           SINK(TESLockChangedEvent),
                           SINK(TESMagicEffectApplyEvent),
                           SINK(TESMagicWardHitEvent),
                           SINK(TESMoveAttachDetachEvent),
                           SINK(TESOpenCloseEvent),
                           SINK(TESPackageEvent),
                           SINK(TESPerkEntryRunEvent),*/
                          SINK(TESQuestInitEvent),
                          SINK(TESQuestStageEvent),
                          SINK(TESQuestStageItemDoneEvent),
                          SINK(TESQuestStartStopEvent),
                          /*SINK(TESResetEvent),
                          SINK(TESSceneEvent),
                          SINK(TESSceneActionEvent),
                          SINK(TESScenePhaseEvent),
                          SINK(TESSellEvent),
                          SINK(TESSleepStartEvent),
                          SINK(TESSleepStopEvent),
                          SINK(TESSpellCastEvent),
                          SINK(TESSwitchRaceCompleteEvent),
                          SINK(TESPlayerBowShotEvent),
                          SINK(TESTopicInfoEvent),
                          SINK(TESTrackedStatsEvent),
                          SINK(TESTrapHitEvent),
                          SINK(TESTriggerEvent),
                          SINK(TESTriggerEnterEvent),
                          SINK(TESTriggerLeaveEvent),
                          SINK(TESUniqueIDChangeEvent),
                          SINK(TESWaitStartEvent),*/
                          SINK(TESWaitStopEvent) {
 public:
  void OnEvent(const PlayerCellChangeEvent& event) override {
    if (event.isExterior && !event.isChangedInOut && event.oldCell != 0) {
      if (auto* oldCellForm = RE::TESForm::LookupByID(event.oldCell)) {
        auto* oldCell = oldCellForm->As<RE::TESObjectCELL>();
        ApplyConditionsToRefsInCell(oldCell);
      }
    }
    ApplyConditionsToRefsInCell(
        RE::PlayerCharacter::GetSingleton()->GetParentCell());
  }

  void OnEvent(const ArmorAttachEvent& event) override {
    ApplyConditionsToRef(event.actor, 1);
  }

  void OnEvent(const WeatherChangeEvent& event) override {
    ApplyConditionsToRefsInCell(
        RE::PlayerCharacter::GetSingleton()->GetParentCell());
  }

  /*SINK_HANDLE(TESActivateEvent) { CONT; }
  SINK_HANDLE(TESActiveEffectApplyRemoveEvent) { CONT; }
  SINK_HANDLE(TESActorLocationChangeEvent) { CONT; }
  SINK_HANDLE(TESBookReadEvent) { CONT; }
  SINK_HANDLE(TESCombatEvent) { CONT; }
  SINK_HANDLE(TESContainerChangedEvent) { CONT; }
  SINK_HANDLE(TESDeathEvent) { CONT; }
  SINK_HANDLE(TESDestructionStageChangedEvent) { CONT; }
  SINK_HANDLE(TESEnterBleedoutEvent) { CONT; }
  SINK_HANDLE(TESEquipEvent) { CONT; }
  SINK_HANDLE(TESFastTravelEndEvent) { CONT; }
  SINK_HANDLE(TESFurnitureEvent) { CONT; }
  SINK_HANDLE(TESGrabReleaseEvent) { CONT; }
  SINK_HANDLE(TESHitEvent) { CONT; }
  SINK_HANDLE(TESLockChangedEvent) { CONT; }
  SINK_HANDLE(TESMagicEffectApplyEvent) { CONT; }
  SINK_HANDLE(TESMagicWardHitEvent) { CONT; }
  SINK_HANDLE(TESMoveAttachDetachEvent) { CONT; }
  SINK_HANDLE(TESOpenCloseEvent) { CONT; }
  SINK_HANDLE(TESPackageEvent) { CONT; }
  SINK_HANDLE(TESPerkEntryRunEvent) { CONT; }
  SINK_HANDLE(TESPlayerBowShotEvent) { CONT; }*/
  SINK_HANDLE(TESQuestInitEvent) { CONT; }
  SINK_HANDLE(TESQuestStageEvent) { CONT; }
  SINK_HANDLE(TESQuestStageItemDoneEvent) { CONT; }
  SINK_HANDLE(TESQuestStartStopEvent) { CONT; }
  /*SINK_HANDLE(TESResetEvent) { CONT; }
  SINK_HANDLE(TESSceneActionEvent) { CONT; }
  SINK_HANDLE(TESSceneEvent) { CONT; }
  SINK_HANDLE(TESScenePhaseEvent) { CONT; }
  SINK_HANDLE(TESSellEvent) { CONT; }
  SINK_HANDLE(TESSleepStartEvent) { CONT; }
  SINK_HANDLE(TESSleepStopEvent) { CONT; }
  SINK_HANDLE(TESSpellCastEvent) { CONT; }
  SINK_HANDLE(TESSwitchRaceCompleteEvent) { CONT; }
  SINK_HANDLE(TESTopicInfoEvent) { CONT; }
  SINK_HANDLE(TESTrackedStatsEvent) { CONT; }
  SINK_HANDLE(TESTrapHitEvent) { CONT; }
  SINK_HANDLE(TESTriggerEnterEvent) { CONT; }
  SINK_HANDLE(TESTriggerLeaveEvent) { CONT; }
  SINK_HANDLE(TESUniqueIDChangeEvent) { CONT; }
  SINK_HANDLE(TESWaitStartEvent) { CONT; }*/
  SINK_HANDLE(TESWaitStopEvent) {
    ApplyConditionsToRefsInCell(
        RE::PlayerCharacter::GetSingleton()->GetParentCell());
    CONT;
  }

  void RegisterEventSinks() {
    g_armorAttachSource.AddListener(this);
    g_cellChangeSource.AddListener(this);
    g_weatherChangeSource.AddListener(this);
    auto* vm = RE::ScriptEventSourceHolder::GetSingleton();
#define ADD_EVENT_SINK(_T) vm->AddEventSink<RE::_T>(this)
    ADD_EVENT_SINK(TESQuestInitEvent);
    ADD_EVENT_SINK(TESQuestStageEvent);
    ADD_EVENT_SINK(TESQuestStageItemDoneEvent);
    ADD_EVENT_SINK(TESWaitStopEvent);
  }

 private:
  static void ApplyConditionsToRef(RE::TESObjectREFR* refr, int delay) {
    if (!refr) {
      return;
    }
    // first do ref materials
    MaterialLoader::VisitMaterialFilesForFormID(
        refr->GetBaseObject()->GetFormID(),
        [&refr, delay](const MATC& config) {
          if (config.conditions.empty()) {
            _TRACE("No conditions found for material {}", config.name);
            return RE::BSVisit::BSVisitControl::kContinue;
          }
          _TRACE("Checking conditions for material {}", config.name);
          // TODO: memoize the condition check
          auto didPass = true;
          for (auto condition : config.conditions) {
            if (!didPass) {
              break;
            }
            if (condition.Evaluate(refr) == !condition.negate) {
              _TRACE("Condition passed: {} {}", condition.type,
                     condition.value.dump().value());
              didPass = true;
            } else {
              _TRACE("Condition failed: {} {}", condition.type,
                     condition.value.dump().value());
              didPass = false;
            }
          }
          if (didPass) {
            TaskManager::GetSingleton()->RegisterDelayTask(
                [refr, config] {
                  MaterialManager::ApplyMaterialToRefr(refr, &config);
                },
                delay);
          }
          return RE::BSVisit::BSVisitControl::kContinue;
        });
    if (const auto* actor = refr->As<RE::Actor>()) {
      // it's an actor, proc armor
    }
  }

  static void ApplyConditionsToRefsInCell(RE::TESObjectCELL* cell) {
    cell->ForEachReference([](RE::TESObjectREFR* refr) {
      ApplyConditionsToRef(
          refr, Options::GetSingleton()->GetApplyMaterialTickDelay() + 5);
      return RE::BSContainer::ForEachResult::kContinue;
    });
  }
};
}  // namespace Conditions