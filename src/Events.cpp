#include "Events.h"

#include "MaterialWriter.h"

RE::BSEventNotifyControl Events::ProcessEvent(
    const RE::TESEquipEvent* event,
    RE::BSTEventSource<RE::TESEquipEvent>* src) {
  if (event->equipped) {
    RE::NiPointer actorRef(event->actor);
    SKSE::GetTaskInterface()->AddTask([actorRef] {
      MaterialWriter::ApplySavedMaterials(actorRef);
    });
  }
  
  return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl Events::ProcessEvent(
    const RE::TESLoadGameEvent* event,
    RE::BSTEventSource<RE::TESLoadGameEvent>* src) {
  MaterialWriter::ApplySavedMaterials(RE::PlayerCharacter::GetSingleton());
  return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl Events::ProcessEvent(
    const SKSE::ModCallbackEvent* event,
    RE::BSTEventSource<SKSE::ModCallbackEvent>* src) {
  return RE::BSEventNotifyControl::kContinue;
}