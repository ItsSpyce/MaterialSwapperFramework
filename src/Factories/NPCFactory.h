#pragma once
#include "Save/Save.h"

namespace Factories {
class NPCFactory : public Singleton<NPCFactory>, public ISaveable {
 public:
  bool ApplyMaterial(RE::Actor* npc, const MaterialConfig& material);
  bool ApplyDefaultMaterial(RE::Actor* npc);
  bool ApplySavedMaterial(RE::Actor* npc);

  // ISaveable interface
  void ReadFromSave(SKSE::SerializationInterface* iface,
                    Save::SaveData& saveData) override;
  void WriteToSave(SKSE::SerializationInterface* iface,
                   Save::SaveData& saveData) override;
};

}  // namespace Factories