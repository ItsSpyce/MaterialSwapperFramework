#include "Events.h"

#include "MaterialWriter.h"

RE::BSEventNotifyControl
Events::ProcessEvent(
    const RE::TESEquipEvent* event,
    RE::BSTEventSource<RE::TESEquipEvent>* src) {
  /*MaterialWriter::ApplyDefaultMaterial(
      RE::PlayerCharacter::GetSingleton(), RE::BIPED_OBJECTS::BIPED_OBJECT::kBody);*/
  return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl
Events::ProcessEvent(
    const RE::TESLoadGameEvent* event,
    RE::BSTEventSource<RE::TESLoadGameEvent>* src) {
  /*MaterialWriter::ApplyDefaultMaterial(
      RE::PlayerCharacter::GetSingleton(), RE::BIPED_OBJECTS::BIPED_OBJECT::kBody);*/
  return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl Events::ProcessEvent(
    const SKSE::ModCallbackEvent* event,
    RE::BSTEventSource<SKSE::ModCallbackEvent>* src) {
  return RE::BSEventNotifyControl::kContinue;
}