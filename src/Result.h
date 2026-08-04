#pragma once

#include <expected>

template <class _Ty>
class _NODISCARD result : public std::expected<_Ty, std::runtime_error> {
  result(const std::optional<_Ty>& _ty, const std::optional<std::string> _err)
      : expected<_Ty, std::runtime_error>(_ty, _err) {}
};

template <class _Ty, typename... Args>
result<_Ty> Err(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  return result(std::nullopt, fmt::format(fmt, std::forward<Args>(args)...));
}

template <class _Ty>
result<_Ty> Ok(const _Ty& _ty) {
  return result(_ty, std::nullopt);
}