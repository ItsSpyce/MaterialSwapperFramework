#pragma once
#include "MSFEvent.h"

struct MaterialRecord;

namespace Events {
class PreMaterialChangeEvent : public MSFEvent {
 public:
  PreMaterialChangeEvent(const RE::Actor* actor, const MaterialRecord* material, const RE::TESForm* form)
      : actor_(actor), material_(material), form_(form) {}

  NODISCARD const RE::Actor* GetActor() const { return actor_; }
  NODISCARD const MaterialRecord* GetMaterial() const { return material_; }

 private:
  const RE::Actor* actor_;
  const MaterialRecord* material_;
  const RE::TESForm* form_;
};
}  // namespace Events