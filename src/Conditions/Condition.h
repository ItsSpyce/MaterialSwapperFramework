#pragma once

#include <glaze/glaze.hpp>

namespace Conditions {
struct DayOfWeekCondition;

struct Condition {
  string type;
  optional<string> mathOp;
  bool negate{false};
  glz::json_t value;

  bool Evaluate(RE::TESObjectREFR* refr) const {
    return negate ? !EvaluateImpl(refr) : EvaluateImpl(refr);
  }

  void Render() const;

 private:
  bool EvaluateImpl(RE::TESObjectREFR* refr) const;
};
}  // namespace Conditions