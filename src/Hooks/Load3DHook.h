#pragma once

#include "Factories.h"

namespace Hooks {
struct Load3DHook {
  static RE::NiAVObject* thunk(RE::Actor* actor, bool backgroundLoading) {
    auto* node = func(actor, backgroundLoading);
    logger::debug("Load3DHook: Actor: {}, Node: {}", actor->GetFormID(),
                  node ? node->name : "nullptr");
    Factories::ArmorFactory::GetSingleton()->ApplySavedMaterialsImmediate(actor,
                                                                          node);
    return node;
  }

  static inline REL::Relocation<decltype(thunk)> func;
};

struct Actor_Set3D {
  static void thunk(RE::Actor* actor, RE::NiAVObject* node,
                  bool queue3DTasks) {
    logger::debug("Actor_Set3D: Actor: {}, Node: {}", actor->GetFormID(),
                  node ? node->name : "nullptr");
    func(actor, node, queue3DTasks);
  }

  static inline REL::Relocation<decltype(thunk)> func;
  static inline auto idx = 0x06C;  // Set3D index in Actor vtable
};

struct Actor_FinishLoadGame {
  static void thunk(RE::Actor* actor, RE::BGSLoadFormBuffer* buf) {
    logger::debug("Actor_FinishLoadGame: Actor: {}", actor->GetFormID());
    func(actor, buf);
  }

  static inline REL::Relocation<decltype(thunk)> func;
  static inline auto idx = 0x011;  // FinishLoadGame index in Actor vtable
};

struct Actor_InitHavok {
  static void thunk(RE::Actor* actor) {
    logger::debug("Actor_InitHavok: Actor: {}", actor->GetFormID());
    func(actor);
  }

  static inline REL::Relocation<decltype(thunk)> func;
  static inline auto idx = 0x066;  // InitHavok index in Actor vtable
};

struct ActorProcess_Update3DModel {
  static void thunk(RE::AIProcess* process, RE::Actor* actor) {
    logger::debug("ActorProcess_Update3DModel: Actor: {}", actor->GetFormID());
    func(process, actor);
  }

  static inline REL::Relocation<decltype(thunk)> func;
};
}  // namespace Hooks