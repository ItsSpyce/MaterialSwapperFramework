#pragma once

class MaterialStream {
 public:
  explicit MaterialStream(IStreamPtr&& stream) : stream_(std::move(stream)) {}

  void Skip(const size_t size) const { stream_->ignore(size); }

  template <class T>
  void Read(T& data) {
    ReadBufferOfType<1>(stream_, &data);
  }

  template <class T, size_t Size>
  void ReadArray(std::array<T, Size>& arr) {
    ReadBufferOfType<Size>(stream_, arr.data());
  }

 private:
  IStreamPtr stream_;

  template <std::size_t NumInstances, typename T>
  void ReadBufferOfType(IStreamPtr& stream, T* dest) {
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
  void ReadBufferOfType(IStreamPtr& stream, T (&dest)[NumInstances]) {
    read_buffer_of_type<NumInstances>(stream, static_cast<T*>(dest));
  }
};

template <>
inline void MaterialStream::Read<Vector2>(Vector2& vec) {
  std::array<float, 2> arr;
  ReadArray(arr);
  vec.x = arr[0], vec.y = arr[1];
}
template <>
inline void MaterialStream::Read<Vector3>(Vector3& vec) {
  std::array<float, 3> arr;
  ReadArray(arr);
  vec.x = arr[0], vec.y = arr[1], vec.z = arr[2];
}
template <>
inline void MaterialStream::Read<Vector4>(Vector4& vec) {
  std::array<float, 4> arr;
  ReadArray(arr);
  vec.x = arr[1], vec.y = arr[1], vec.z = arr[2], vec.w = arr[3];
}
template <>
inline void MaterialStream::Read<std::string>(std::string& str) {
  std::uint32_t length;
  Read(length);
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