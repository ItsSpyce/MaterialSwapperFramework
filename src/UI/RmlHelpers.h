#pragma once

#include <RmlUi/Core.h>

namespace UI::RmlHelpers {
using namespace Rml;

#define CHECK_ARGLEN(_SIZE)                                 \
  if (args.size() != (_SIZE)) return Err {                  \
      "Expected {} argument(s), got {}", _SIZE, args.size() \
    }
#define TRY_INTO(_INDEX)                                        \
  if (!args[((_INDEX) - 1)].GetInto(a##_INDEX)) return Err {    \
      "Incorrect type attempted for parameter index {}", _INDEX \
    }

template <typename A1>
result<std::tuple<A1>> make_tuple_from_args(const VariantList& args) {
  CHECK_ARGLEN(1);
  A1 a1;
  TRY_INTO(1);
  return Ok{std::make_tuple(a1)};
}

template <typename A1, typename A2>
result<std::tuple<A1, A2>> make_tuple_from_args(const VariantList& args) {
  CHECK_ARGLEN(2);
  A1 a1;
  A2 a2;
  TRY_INTO(1);
  TRY_INTO(2);
  return Ok{std::make_tuple(a1, a2)};
}

template <typename A1, typename A2, typename A3>
result<std::tuple<A1, A2, A3>> make_tuple_from_args(const VariantList& args) {
  CHECK_ARGLEN(3);
  A1 a1;
  A2 a2;
  A3 a3;
  TRY_INTO(1);
  TRY_INTO(2);
  TRY_INTO(3);
  return Ok{std::make_tuple(a1, a2, a3)};
}
}  // namespace UI::RmlHelpers