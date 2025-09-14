#pragma once

#include "V1.h"
#include "V2.h"

namespace Save {
using SaveData = V1::SaveData;
void SaveCallback(SKSE::SerializationInterface* iface);

void LoadCallback(SKSE::SerializationInterface* iface);

void RevertCallback(SKSE::SerializationInterface* iface);

bool Install();
}  // namespace Save

class ISaveable {
 public:
  virtual void ReadFromSave(Save::SaveData& saveData) = 0;
  virtual void WriteToSave(Save::SaveData& saveData) const = 0;
};