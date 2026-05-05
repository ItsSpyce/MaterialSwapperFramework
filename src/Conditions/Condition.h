#pragma once

#include <glaze/glaze.hpp>

namespace Conditions {
struct Condition {
  string type;
  optional<string> mathOp;
  bool negate{false};
  glz::generic value;

  bool Evaluate(RE::TESObjectREFR* refr) const;

  void Render() const;
};
}  // namespace Conditions