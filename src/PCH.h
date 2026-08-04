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
#include <detours/detours.h>
#include <d3d11.h>
#include <direct.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <dxcore_interface.h>
#include <dxcore.h>
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

constexpr auto enum_range(auto first, auto last) {
  auto enum_range =
      std::views::iota(std::to_underlying(first), std::to_underlying(last)) |
      std::views::transform(
          [](auto enum_val) { return (decltype(first))enum_val; });
  return enum_range;
}
}  // namespace stl
