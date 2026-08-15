#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define BS_THREAD_POOL_NATIVE_EXTENSIONS

#define MAGIC_ENUM_RANGE_MIN 0
// very bad. This is ONLY because of function IDs.
#define MAGIC_ENUM_RANGE_MAX 736

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <REX/REX/Singleton.h>
#include <SKSE/SKSE.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <wrl/client.h>
#include <concurrent_vector.h>

#include <ranges>
#include <spdlog/sinks/msvc_sink.h>

template <class T>
using Singleton = REX::Singleton<T>;

using Microsoft::WRL::ComPtr;

namespace logger = SKSE::log;
namespace fs = std::filesystem;
using namespace std;
using namespace std::literals;

constexpr auto enum_range(auto first, auto last) {
  auto enum_range =
      std::views::iota(std::to_underlying(first), std::to_underlying(last)) |
      std::views::transform(
          [](auto enum_val) { return (decltype(first))enum_val; });
  return enum_range;
}  // namespace stl
