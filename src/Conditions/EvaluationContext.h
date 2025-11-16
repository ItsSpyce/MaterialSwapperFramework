#pragma once

#include "Condition.h"
#include "Events/EventListener.h"
#include "IO/MaterialLoader.h"
#include "MaterialHelpers.h"
#include "TaskManager.h"
#include "ThreadPool.h"

extern unique_ptr<ThreadPool> g_updateThreads;

namespace Conditions {
class EvaluationContext : public Singleton<EvaluationContext>,
                          public EventListener<PlayerCellChangeEvent>,
                          public EventListener<ArmorAttachEvent>,
                          public EventListener<WeatherChangeEvent>,
                          public RE::BSTEventSink<RE::TESWaitStopEvent> {
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

  RE::BSEventNotifyControl ProcessEvent(
      const RE::TESWaitStopEvent* a_event,
      RE::BSTEventSource<RE::TESWaitStopEvent>* a_eventSource) override {
    ApplyConditionsToRefsInCell(
        RE::PlayerCharacter::GetSingleton()->GetParentCell());
    return RE::BSEventNotifyControl::kContinue;
  }

 private:
  static void ApplyConditionsToRef(RE::TESObjectREFR* refr, int delay) {
    if (!refr) {
      return;
    }
    optional<Condition> match;
    // first do ref materials
    MaterialLoader::VisitMaterialFilesForFormID(
        refr->GetBaseObject()->GetFormID(),
        [&refr, delay](const MaterialConfig& config) {
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
            if (condition.Evaluate(refr)) {
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
                  MaterialHelpers::ApplyMaterialToRefr(refr, &config);
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