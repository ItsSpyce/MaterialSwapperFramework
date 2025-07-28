#pragma once

#include "V1.h"

namespace Save {
using SaveData = V1::SaveData;
void SaveCallback(SKSE::SerializationInterface* iface);

void LoadCallback(SKSE::SerializationInterface* iface);

void RevertCallback(SKSE::SerializationInterface* iface);

bool Install();
}  // namespace Save