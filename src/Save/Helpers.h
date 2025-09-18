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

template <typename T>
uint32_t ReadJsonObject(SKSE::SerializationInterface* iface, T& out) {
  size_t jsonLength;
  if (!iface->ReadRecordData(jsonLength)) {
    _ERROR("Failed to read JSON size from serialization interface");
    return 0;
  }
  string json(jsonLength, '\0');
  if (!iface->ReadRecordData(json.data(), jsonLength)) {
    _ERROR("Failed to read JSON data from serialization interface");
    return 0;
  }
  _DEBUG("Deserializing from JSON: {}", json);
  if (auto err = glz::read_json(out, json)) {
    auto cleanedError = glz::format_error(err);
    _ERROR("Failed to read object from JSON: {}", cleanedError);
    return 0;
  }
  return jsonLength;
}

template <typename T>
bool WriteJsonObject(SKSE::SerializationInterface* iface, const T& obj) {
  string json{};
  if (auto err = glz::write_json(obj, json)) {
    auto cleanedError = glz::format_error(err);
    _ERROR("Failed to write object to JSON: {}", cleanedError);
    return false;
  }
  _DEBUG("Serialized to JSON: {}", json);
  if (!iface->WriteRecordData(json.size())) {
    _ERROR("Failed to write JSON size to serialization interface");
    return false;
  }
  if (!iface->WriteRecordData(json.data(), json.size())) {
    _ERROR("Failed to write JSON data to serialization interface");
    return false;
  }
  return true;
}
}  // namespace Helpers
}  // namespace Save