#pragma once

#include "MathTypes.h"

class MaterialStream {
 public:
  explicit MaterialStream(IStreamPtr&& stream) : stream_(std::move(stream)) {}

  void skip(const size_t size) { stream_->ignore(size); }

  template <class T>
  void read(T& data) {
    read_buffer_of_type<1>(stream_, &data);
  }

  template <class T, size_t Size>
  void read_array(std::array<T, Size>& arr) {
    read_buffer_of_type<Size>(stream_, arr.data());
  }

 private:
  IStreamPtr stream_;

  template <std::size_t NumInstances, typename T>
  void read_buffer_of_type(IStreamPtr& stream, T* dest) {
    static_assert(std::is_arithmetic_v<T>,
                  "Buffer element type is not arithmetic");
    stream->read(reinterpret_cast<char*>(dest), NumInstances * sizeof(T));
    if (stream->bad()) {
      throw std::runtime_error(
          fmt::format("Failed to read typed ({0}) buffer of {1} instances",
                      typeid(T).name(), NumInstances));
    }
  }

  template <std::size_t NumInstances, typename T>
  void read_buffer_of_type(IStreamPtr& stream, T (&dest)[NumInstances]) {
    read_buffer_of_type<NumInstances>(stream, static_cast<T*>(dest));
  }
};

template <>
inline void MaterialStream::read<Vector2>(Vector2& vec) {
  std::array<float, 2> arr;
  read_array(arr);
  vec.x = arr[0], vec.y = arr[1];
}
template <>
inline void MaterialStream::read<Vector3>(Vector3& vec) {
  std::array<float, 3> arr;
  read_array(arr);
  vec.x = arr[0], vec.y = arr[1], vec.z = arr[2];
}
template <>
inline void MaterialStream::read<Vector4>(Vector4& vec) {
  std::array<float, 4> arr;
  read_array(arr);
  vec.x = arr[1], vec.y = arr[1], vec.z = arr[2], vec.w = arr[3];
}
template <>
inline void MaterialStream::read<std::string>(std::string& str) {
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