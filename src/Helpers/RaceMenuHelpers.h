#pragma once
#include "SKEE64.h"
#include "SKEE64PluginInterface.h"

namespace Helpers {
inline UniqueID GetUniqueID(RE::TESObjectREFR* refr,
                            const RE::InventoryEntryData* data, bool init) {
  if (!refr || !data) {
    return NULL;
  }
  if (auto* armo = data->object->As<RE::TESObjectARMO>()) {
    if (auto* itemDataInterface = SKEE64Instance::GetSingleton()
                                      ->Query<SKEE64::IItemDataInterface>()) {
      SKEE64::IItemDataInterface::Identifier iden;
      iden.SetSlotMask(static_cast<SKEE64::skee_u32>(*armo->GetSlotMask()), 0);
      return itemDataInterface->GetItemUniqueID(refr, iden, init);
    }
  }

  return NULL;
}

inline UniqueID GetUniqueID(RE::TESObjectREFR* refr,
                            RE::BGSBipedObjectForm::BipedObjectSlot slot,
                            bool init) {
  if (!refr) {
    return NULL;
  }
  if (auto* itemDataInterface =
          SKEE64Instance::GetSingleton()->Query<SKEE64::IItemDataInterface>()) {
    SKEE64::IItemDataInterface::Identifier iden;
    iden.SetSlotMask(static_cast<SKEE64::skee_u32>(slot), 0);
    return itemDataInterface->GetItemUniqueID(refr, iden, init);
  }
  return NULL;
}

inline UniqueID GetUniqueID(RE::TESObjectREFR* refr, RE::BIPED_OBJECT slot,
                            bool init) {
  if (!refr) {
    return NULL;
  }
  if (auto* itemDataInterface =
          SKEE64Instance::GetSingleton()->Query<SKEE64::IItemDataInterface>()) {
    SKEE64::IItemDataInterface::Identifier iden;
    if (slot > RE::BIPED_OBJECT::kEditorTotal) {
      iden.SetSlotMask(0, slot);
    } else {
      iden.SetSlotMask(slot, 0);
    }
    return itemDataInterface->GetItemUniqueID(refr, iden, init);
  }
  return NULL;
}

inline RE::TESForm* GetFormForUniqueID(UniqueID uid) {
  if (uid == NULL) {
    return nullptr;
  }
  if (auto* itemDataInterface =
          SKEE64Instance::GetSingleton()->Query<SKEE64::IItemDataInterface>()) {
    return itemDataInterface->GetFormFromUniqueID(uid);
  }
  return nullptr;
}

inline RE::FormID GetFormIDForUniqueID(UniqueID uid) {
  const auto* form = GetFormForUniqueID(uid);
  return form ? form->GetFormID() : NULL;
}

struct InventoryItem {
  RE::TESBoundObject* object;
  i32 count;
  std::unique_ptr<RE::InventoryEntryData> data;
  UniqueID uid;
};

}  // namespace Helpers