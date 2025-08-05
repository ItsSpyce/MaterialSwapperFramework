#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define BS_THREAD_POOL_NATIVE_EXTENSIONS

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>
#include <d3d11.h>
#include <dxgi.h>
#include <fmt/ostream.h>

#include <algorithm>
#include <cctype>
#include <locale>
#include <future>
#include <Singleton.h>
#include <bs_thread_pool.hpp>

namespace logger = SKSE::log;
using namespace std;
using namespace std::literals;
using IStreamPtr = unique_ptr<istream>;

namespace stl {
using namespace SKSE::stl;

template <class T>
void write_thunk_call(uintptr_t a_src) {
  SKSE::AllocTrampoline(14);

  auto& trampoline = SKSE::GetTrampoline();
  T::func = trampoline.write_call<5>(a_src, T::thunk);
}

template <class F, class T>
void write_vfunc() {
  REL::Relocation vtbl{F::VTABLE[0]};
  T::func = vtbl.write_vfunc(T::idx, T::thunk);
}
}  // namespace stl

#define RETURN_IF_FALSE(_VAR)                     \
  if (!(_VAR)) {                                  \
    logger::error("Failed condition: {}", #_VAR); \
    return false;                                 \
  }

#ifdef SKYRIM_AE
#define OFFSET(se, ae) ae
#else
#define OFFSET(se, ae) se
#endif