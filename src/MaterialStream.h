#pragma once

#include "MathTypes.h"

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
};

template <>
void MaterialStream::read<Vector2>(Vector2& vec);
template<>
void MaterialStream::read<Vector3>(Vector3& vec);
template<>
void MaterialStream::read<Vector4>(Vector4& vec);
template<>
void MaterialStream::read<std::string>(std::string& str);