#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define BS_THREAD_POOL_NATIVE_EXTENSIONS

#define UI_USE_IMGUI

#ifdef UI_USE_NUKLEAR
#define NK_IMPLEMENTATION
#define NK_D3D11_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_STANDARD_VARARGS
#define NK_STANDARD_BOOL
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_UINT_DRAW_INDEX
#define NK_MAX_NUMBER_BUFFER UINT16_MAX
#define NK_PRIVATE
#define STBTT_STATIC
#define STBRP_STATIC
#endif

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
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>

#include <ranges>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include "hash_table8.hpp"


template <class T>
using Singleton = REX::Singleton<T>;

using Microsoft::WRL::ComPtr;

namespace logger = SKSE::log;
using namespace std;
using namespace std::literals;
using IStreamPtr = unique_ptr<istream>;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

namespace stl {
using namespace SKSE::stl;

template <class T, size_t size = 14>
void write_thunk_call() {
  SKSE::AllocTrampoline(size);

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

#define RETURN_IF_FALSE(_VAR)                     \
  if (!(_VAR)) {                                  \
    logger::error("Failed condition: {}", #_VAR); \
    return false;                                 \
  }

template <typename... Args>
using Visitor = function<RE::BSVisit::BSVisitControl(Args...)>;

#define RETURN_IF_STOP(_VISITOR, ...)                                  \
  if ((_VISITOR)(__VA_ARGS__) == RE::BSVisit::BSVisitControl::kStop) { \
    return;                                                            \
  }

#define BREAK_IF_STOP(_VISITOR, ...)                                   \
  if ((_VISITOR)(__VA_ARGS__) == RE::BSVisit::BSVisitControl::kStop) { \
    break;                                                             \
  }

// logging macros

#define _ERROR(...)            \
  logger::error(__FUNCTION__); \
  logger::error(__VA_ARGS__)
#define _WARN(...) logger::warn(__VA_ARGS__)
#define _INFO(...) logger::info(__VA_ARGS__)
#define _DEBUG(...) logger::debug(__VA_ARGS__)
#define _TRACE(...) logger::trace(__VA_ARGS__)

// various helpers

#define NODISCARD [[nodiscard]]
#define FORCEINLINE __forceinline
#define NOINLINE __declspec(noinline)
#define MAYBE_UNUSED [[maybe_unused]]
#define DEPRECATED(_MSG) [[deprecated(_MSG)]]
#define UNUSED(_VAR) (void)(_VAR)
#define ALIGNAS(_N) alignas(_N)
#define ALIGNOF(_T) alignof(_T)
#define RESTRICT __restrict
#define PACKED(_N) __declspec(align(1)) _N
#define LIKELY(_X) __builtin_expect(!!(_X), 1)
#define UNLIKELY(_X) __builtin_expect(!!(_X), 0)
#define SIZEOF_ARRAY(_ARR) (sizeof(_ARR) / sizeof(_ARR[0]))
#define OFFSET_OF(_TYPE, _MEMBER) offsetof(_TYPE, _MEMBER)
#define FIELD_SIZE(_TYPE, _MEMBER) sizeof(((_TYPE*)0)->_MEMBER)
#define BITFIELD(_N) :_N
#define NOT_IMPLEMENTED                                   \
  {                                                       \
    _ERROR("Function not implemented: {}", __FUNCTION__); \
    throw std::exception("Function not implemented");     \
  }

#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"