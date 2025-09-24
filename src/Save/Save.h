#pragma once

#include "V1.h"
#include "V2.h"

namespace Save {
using SaveData = V2::SaveData;
void SaveCallback(SKSE::SerializationInterface* iface);

void LoadCallback(SKSE::SerializationInterface* iface);

void RevertCallback(SKSE::SerializationInterface* iface);

bool Install();
}  // namespace Save

class ISaveable {
 public:
  virtual void ReadFromSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) = 0;
  virtual void WriteToSave(SKSE::SerializationInterface* iface, Save::SaveData& saveData) = 0;
};