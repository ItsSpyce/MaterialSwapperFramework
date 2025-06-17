#include "MaterialStream.h"

template <>
void MaterialStream::read<Vector2>(Vector2& vec) {
  std::array<float, 2> arr;
  read_array(arr);
  vec.x = arr[0], vec.y = arr[1];
}

template <>
void MaterialStream::read<Vector3>(Vector3& vec) {
  std::array<float, 3> arr;
  read_array(arr);
  vec.x = arr[0], vec.y = arr[1], vec.z = arr[2];
}

template <>
void MaterialStream::read<Vector4>(Vector4& vec) {
  std::array<float, 4> arr;
  read_array(arr);
  vec.x = arr[1], vec.y = arr[1], vec.z = arr[2], vec.w = arr[3];
}

template <>
void MaterialStream::read<std::string>(std::string& str) {
  std::uint32_t length;
  read(length);
  if (length > 1024) {
    throw std::runtime_error(
        fmt::format("Requested string length is too large: {0}", length));
  }
  str = std::string(length, '\0');
  stream_->read(str.data(), length);
  if (stream_->bad()) {
    throw std::runtime_error(
        fmt::format("Failed to read sized string of {0} chars", length));
  }
  if (const auto end = str.find('\0'); end != std::string::npos) {
    str.erase(end);
  }
}