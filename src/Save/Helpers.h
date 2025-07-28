#pragma once

namespace Save {
namespace Helpers {

template <typename T>
bool WriteData(const SKSE::SerializationInterface* serialization,
               const T* data) {
  return serialization->WriteRecordData(data, sizeof(T));
}

template <typename T>
bool ReadData(const SKSE::SerializationInterface* serialization, T* data) {
  return serialization->ReadRecordData(data, sizeof(T)) > 0;
}

template <>
inline bool WriteData<RE::BSFixedString>(
    const SKSE::SerializationInterface* serialization,
    const RE::BSFixedString* str) {
  uint32_t len = str->length();
  if (!serialization->WriteRecordData(&len, sizeof(len))) {
    return false;
  }
  if (len != 0 && serialization->WriteRecordData(str->c_str(), len)) {
    return true;
  }
  return false;
}
template <>
inline bool ReadData<RE::BSFixedString>(
    const SKSE::SerializationInterface* serialization, RE::BSFixedString* str) {
  uint32_t len = 0;
  if (!serialization->ReadRecordData(&len, sizeof(len))) {
    logger::error("Failed to read BSFixedString length");
    return false;
  }
  if (len == 0) {
    str = new RE::BSFixedString();
    return true;
  }
  std::string buffer(len, '\0');
  if (serialization->ReadRecordData(buffer.data(), len) > 0) {
    str = new RE::BSFixedString(buffer.c_str());
    return true;
  }
  logger::error("Failed to read BSFixedString data");
  return false;
}
}  // namespace Helpers
}  // namespace Save