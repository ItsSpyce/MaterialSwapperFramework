#include "Conditions/ConditionParser.h"

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

using OpCode = RE::CONDITION_ITEM_DATA::OpCode;

TEST_CASE("Validate condition string", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("GetStage MyQuest == 50");
  if (condition.is_err()) {
    INFO(fmt::format("Error: {}", condition.error()));
  }
  REQUIRE(condition.is_ok());
  REQUIRE(condition.value().function == MaterialFunctionID::GetStage);
  REQUIRE(condition.value().variable == "MyQuest");
  REQUIRE(condition.value().op == OpCode::kEqualTo);
  REQUIRE(std::get<int>(condition.value().param.value()) == 50);
}

TEST_CASE("Validates boolean condition", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("IsMoving");
  REQUIRE(condition.is_ok());
  REQUIRE(condition.value().function == MaterialFunctionID::IsMoving);
  REQUIRE(condition.value().variable.empty());
  REQUIRE(condition.value().op == OpCode::kEqualTo);
  REQUIRE(!condition.value().param.has_value());
}

TEST_CASE("Validates string condition", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("GetStage MyQuest == 'some string'");
  REQUIRE(condition.is_ok());
  REQUIRE(std::get<std::string>(condition.value().param.value()) == "some string");
}

TEST_CASE("Permits self-targeted or global variables", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("GetIsCurrentWeather == 0");
  REQUIRE(condition.is_ok());
  REQUIRE(condition.value().function == MaterialFunctionID::GetIsCurrentWeather);
  REQUIRE(condition.value().variable.empty());
  REQUIRE(condition.value().op == OpCode::kEqualTo);
  REQUIRE(std::get<int>(condition.value().param.value()) == 0);
}

TEST_CASE("Errors when condition function not found", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("NotValid MyQuest == 100");
  REQUIRE(condition.is_err());
  REQUIRE(condition.error() == "Unknown condition function: NotValid");
}

TEST_CASE("Errors when condition has variables but no op code", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("GetStage MyQuest");
  REQUIRE(condition.is_err());
  REQUIRE(condition.error() == "Invalid condition string");
}

TEST_CASE("Errors when condition param has incorrect strings", "[ParseFromString]") {
  const auto condition = Conditions::ParseFromString("GetActorValue SomeString == 'my string");
  REQUIRE(condition.is_err());
  REQUIRE(condition.error() == "Malformed string. Expected quote terminator (SINGLE_QUOTE)");
}
