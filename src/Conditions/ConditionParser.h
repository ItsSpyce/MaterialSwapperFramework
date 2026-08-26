#pragma once

#include <magic_enum/magic_enum.hpp>
#include <srell.hpp>

#include "Helpers/StringHelpers.h"

namespace Conditions {
static result<MaterialCondition> ParseFromString(const char* str) {
  using OpCode = RE::CONDITION_ITEM_DATA::OpCode;
  // insert stupid fucking meme of "AST vs Regex" here
  static srell::regex CONDITION_REGEX{
      R"(^(\w+)\s*(?:([A-Za-z0-9_$-]+)?\s*([=!><]{1,2})\s*(.+))?$)"};
  static srell::cmatch MATCH;
  if (str[0] == '\0') return Err{"Condition string is empty"};
  if (srell::regex_search(str, MATCH, CONDITION_REGEX)) {
    auto funcStr = MATCH[1].str();
    const auto func =
        magic_enum::enum_cast<MaterialFunctionID>(funcStr).value_or(
            MaterialFunctionID::NONE);
    if (func == MaterialFunctionID::NONE) {
      return Err{"Unknown condition function: {}", funcStr};
    }
    std::string variable;
    auto op = OpCode::kEqualTo;
    MaterialConditionParam param = std::nullopt;
    variable = MATCH[2].str();
    if (const auto opStr = MATCH[3].str(); !opStr.empty()) {
      if (opStr[0] == '!') {
        if (opStr.size() != 2) {
          return Err{"Unexpected end of op code. Expected '!='"};
        }
        if (opStr[1] == '=') {
          op = OpCode::kNotEqualTo;
        } else {
          return Err{"Unexpected op code. Expected '!=', got '!{}'", opStr[1]};
        }
      } else if (opStr[0] == '=') {
        if (opStr[1] == '=') {
          op = OpCode::kEqualTo;
        } else {
          return Err{"Unexpected op code. Expected '==', got '={}'", opStr[1]};
        }
      } else if (opStr[0] == '>') {
        if (opStr.size() == 1) {
          op = OpCode::kGreaterThan;
        } else if (opStr[1] == '=') {
          op = OpCode::kGreaterThanOrEqualTo;
        } else {
          return Err{"Unexpected op code. Expected '>' or '>=', got '>{}'",
                     opStr[1]};
        }
      } else if (opStr[0] == '<') {
        if (opStr.size() == 1) {
          op = OpCode::kLessThan;
        } else if (opStr[1] == '=') {
          op = OpCode::kLessThanOrEqualTo;
        } else {
          return Err{"Unexpected op code. Expected '<' or '<=', got '<{}'",
                     opStr[1]};
        }
      } else {
        return Err{
            "Unexpected op code. Expected '==', '!=', '>', '>=', '<' or '<=', "
            "got '{}'",
            opStr};
      }
    }
    if (const auto paramStr = MATCH[4].str(); !paramStr.empty()) {
      if (paramStr[0] == '\'') {
        // string
        if (paramStr.back() != '\'') {
          return Err{
              "Malformed string. Expected quote terminator (SINGLE_QUOTE)"};
        }
        param = paramStr.substr(1, paramStr.size() - 2);
      } else if (paramStr[0] == '"') {
        if (paramStr.back() != '"') {
          return Err{
              "Malformed string. Expected quote terminator (DOUBLE_QUOTE)"};
        }
        param = paramStr.substr(1, paramStr.size() - 2);
      } else {
        // bool
        if (const auto lowered = StringHelpers::ToLower(paramStr);
            lowered == "true") {
          param = true;
        } else if (lowered == "false") {
          param = false;
        } else {
          // double
          if (const auto d = std::stod(paramStr);
              std::to_string(d) == paramStr) {
            param = d;
          } else {
            // int
            if (const auto i = std::stoi(paramStr);
                std::to_string(i) == paramStr) {
              param = i;
            } else {
              return Err{"Unexpected parameter: {}", paramStr};
            }
          }
        }
      }
    }

    return Ok{MaterialCondition{
        .function = func, .variable = variable, .op = op, .param = param}};
  }
  return Err{"Invalid condition string"};
}
}  // namespace Conditions