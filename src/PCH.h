#pragma once

#include <ranges>
#include <spdlog/sinks/msvc_sink.h>

#include "Types.h"

using namespace std::literals;

constexpr auto enum_range(auto first, auto last) {
  auto enum_range =
      std::views::iota(std::to_underlying(first), std::to_underlying(last)) |
      std::views::transform(
          [](auto enum_val) { return (decltype(first))enum_val; });
  return enum_range;
}
