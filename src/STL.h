#pragma once

#include <detours/detours.h>

#include <expected>

namespace stl {
using namespace SKSE::stl;

template <class T, size_t Size = 14>
void write_thunk_call() {
  SKSE::AllocTrampoline(Size);

  auto& trampoline = SKSE::GetTrampoline();
  T::func =
      trampoline.write_call<5>(T::rel.address() + T::offset.offset(), T::thunk);
}

template <class F, class T>
void write_vfunc() {
  REL::Relocation vtbl{F::VTABLE[0]};
  T::func = vtbl.write_vfunc(T::idx, T::thunk);
}

#ifdef DETOURS_VERSION
template <class F>
void write_detour() {
  DetourAttach(&(PVOID&)F::func, F::thunk);
}
#endif
}  // namespace stl

template <typename... Args>
using Visitor = std::function<RE::BSVisit::BSVisitControl(Args...)>;

template <size_t Bits>
struct _NODISCARD opt_bitset {
  opt_bitset() = default;
  opt_bitset(opt_bitset&& other) noexcept
      : internal_(other.internal_), masked_(other.masked_) {}
  opt_bitset(const opt_bitset& other) noexcept
      : internal_(other.internal_), masked_(other.masked_) {}
  ~opt_bitset() = default;

  template <typename E>
    requires std::is_enum_v<E>
  void set(E bit, bool value) {
    const auto e = std::to_underlying<E>(bit);
    masked_[e] = true;
    masked_[e] = value;
  }

  template <typename E>
    requires std::is_enum_v<E>
  bool get(E bit) const {
    const auto e = std::to_underlying<E>(bit);
    if (masked_[e]) {
      return internal_[e];
    }
    return false;
  }

  template <typename E>
    requires std::is_enum_v<E>
  bool has_value(E bit) const {
    const auto e = std::to_underlying<E>(bit);
    return masked_[e];
  }

  template <typename E>
    requires std::is_enum_v<E>
  bool value_or(E bit, bool other) const {
    const auto e = std::to_underlying<E>(bit);
    if (masked_[e]) {
      return internal_[e];
    }
    return other;
  }

  opt_bitset& operator=(opt_bitset const& rhs) {
    internal_ = rhs.internal_;
    masked_ = rhs.masked_;
    return *this;
  }

  opt_bitset&& operator=(const opt_bitset&& rhs) noexcept {
    internal_ = rhs.internal_;
    masked_ = rhs.masked_;
    return *this;
  }

 private:
  std::bitset<Bits> internal_;
  std::bitset<Bits> masked_;
};

template <typename Func>
  requires std::is_function_v<Func>
struct call_once {
  Func func;
  mutable bool hasCalled = false;
  explicit call_once(const Func& func) : func(func) {}

  template <typename... Args>
  void operator()(Args... args) {
    if (hasCalled) return;
    func(std::forward<Args>(args));
    hasCalled = true;
  }
};

template <typename T>
struct Ok {
  T value;

  template <typename U,
            std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
  Ok(U&& v) : value(std::forward<U>(v)) {}
};

template <std::size_t N>
Ok(const char (&)[N]) -> Ok<std::string>;

template <typename T>
Ok(T&&) -> Ok<std::decay_t<T>>;

template <typename T> struct fmt::formatter<Ok<T>>: formatter<string_view> {
  auto format(Ok<T>& ok, format_context& ctx) const {
    return formatter<string_view>::format(ok.value(), ctx);
  }
};

struct Err {
  std::string error;

  Err(std::string message) : error(std::move(message)) {}
  Err(const char* message) : error(message) {}
  template <typename... Args>
  Err(fmt::format_string<Args...> format, Args&&... args)
      : error(fmt::format(format, std::forward<Args>(args)...)) {}
};

template <> struct fmt::formatter<Err>: formatter<string_view> {
  auto format(Err& err, format_context& ctx) const {
    return formatter<string_view>::format(err.error, ctx);
  }
};

template <typename T>
class result {
 public:
  using value_type = T;
  using error_type = std::string;

 private:
  std::variant<T, std::string> data_;

 public:
  template <typename U,
            std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
  result(Ok<U>&& ok)
      : data_(std::in_place_index<0>, std::forward<U>(ok.value)) {}
  template <typename U,
            std::enable_if_t<std::is_constructible_v<T, const U&>, int> = 0>
  result(const Ok<U>& ok) : data_(std::in_place_index<0>, ok.value) {}
  result(Err&& err) : data_(std::in_place_index<1>, std::move(err.error)) {}
  result(const Err& err) : data_(std::in_place_index<1>, err.error) {}

  [[nodiscard]]
  bool is_ok() const noexcept {
    return data_.index() == 0;
  }

  [[nodiscard]]
  bool is_err() const noexcept {
    return data_.index() == 1;
  }

  explicit operator bool() const noexcept { return is_ok(); }

  T& value() & { return std::get<0>(data_); }

  const T& value() const& { return std::get<0>(data_); }

  T&& value() && { return std::get<0>(std::move(data_)); }

  std::string& error() & { return std::get<1>(data_); }

  const std::string& error() const& { return std::get<1>(data_); }

  std::string&& error() && { return std::get<1>(std::move(data_)); }
};