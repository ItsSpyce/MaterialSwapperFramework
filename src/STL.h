#pragma once

namespace glz {
template <typename T> requires std::is_enum_v<T>
struct meta<T> {
  static constexpr auto keys = magic_enum::enum_names<T>();
  static constexpr auto value = magic_enum::enum_values<T>();
};
}

template <typename... Args>
using Visitor = std::function<RE::BSVisit::BSVisitControl(Args...)>;

template <size_t Bits>
struct _NODISCARD opt_bitset {
  opt_bitset() = default;
  opt_bitset(opt_bitset&& other) noexcept : internal_(other.internal_), masked_(other.masked_) {}
  opt_bitset(const opt_bitset& other) noexcept : internal_(other.internal_), masked_(other.masked_) {}
  ~opt_bitset() = default;

  template <typename E> requires std::is_enum_v<E>
  void set(E bit, bool value) {
    masked_[bit] = true;
    masked_[bit] = value;
  }

  template <typename E> requires std::is_enum_v<E>
  bool get(E bit) const {
    if (masked_[bit]) {
      return &internal_[bit];
    }
    return false;
  }
  
  template <typename E> requires std::is_enum_v<E>
  bool has_value(E bit) const {
    return masked_[bit];
  }

  template <typename E> requires std::is_enum_v<E>
  bool value_or(E bit, bool other) const {
    if (masked_[bit]) {
      return internal_[bit];
    }
    return other;
  }

  opt_bitset& operator=(opt_bitset const& rhs) {
    internal_ = rhs.internal_;
    masked_ = rhs.masked_;
    return *this;
  }

  opt_bitset& operator=(const opt_bitset&& rhs) noexcept {
    internal_ = rhs.internal_;
    masked_ = rhs.masked_;
    return *this;
  }

private:
  std::bitset<Bits> internal_;
  std::bitset<Bits> masked_;
};