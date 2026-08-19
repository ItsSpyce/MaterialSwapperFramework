#pragma once

// Shut up, I'm tired of having these elsewhere

#define FOR_IN_DIR(_RDI, _ITERATOR)                                  \
  for (auto _ITERATOR = fs::begin(_RDI); _ITERATOR != fs::end(_RDI); \
       ++_ITERATOR)

#define FIND_IN(_ITERABLE, _ITERATOR, _FIND)        \
  if (const auto _ITERATOR = _ITERABLE.find(_FIND); \
      _ITERATOR != _ITERABLE.end())

#define RETURN_IF_FALSE(_VAR)                     \
  if (!(_VAR)) {                                  \
    logger::error("Failed condition: {}", #_VAR); \
    return false;                                 \
  }

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
#define SIZEOF_ARRAY(_ARR) (sizeof(_ARR) / sizeof(_ARR[0]))
#define OFFSET_OF(_TYPE, _MEMBER) offsetof(_TYPE, _MEMBER)
#define FIELD_SIZE(_TYPE, _MEMBER) sizeof(((_TYPE*)0)->_MEMBER)
#define BITFIELD(_N) :_N
#define NOT_IMPLEMENTED                                   \
  {                                                       \
    _ERROR("Function not implemented: {}", __FUNCTION__); \
    throw std::exception("Function not implemented");     \
  }
#define NOCOPY(_T)        \
  _T(const _T&) = delete; \
  _T& operator=(const _T&) = delete
#define NOMOVE(_T)   \
  _T(_T&&) = delete; \
  _T& operator=(_T&&) = delete
#define RELEASE(_V) \
  if (_V != nullptr) _V->Release()
// I just learned what this is. Time to abuse it.
#if (defined(_MSC_VER) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L
#define LIKELY(_X) (_X) [[likely]]
#define UNLIKELY(_X) (_X) [[unlikely]]
#elif defined(__GNUC__)
#define LIKELY(_X) (__builtin_expect(!!(_X), 1))
#define UNLIKELY(_X) (__builtin_expect(!!(_X), 0))
#else
#define LIKELY(_X)
#define UNLIKELY(_X)
#endif
#define ENUMI(_V) 1ULL << _V

#define SCOPE_GUARD(_LOCK) std::scoped_lock<std::mutex> guard(_LOCK)

