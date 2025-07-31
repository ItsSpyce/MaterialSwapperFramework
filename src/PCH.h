#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NOUSER
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>
#include <d3d11.h>
#include <dxgi.h>
#include <fmt/ostream.h>

#include <algorithm>
#include <cctype>
#include <locale>

namespace logger = SKSE::log;
using namespace std::literals;
using IStreamPtr = std::unique_ptr<std::istream>;

namespace stl {
using namespace SKSE::stl;

template <class T>
void write_thunk_call(std::uintptr_t a_src) {
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