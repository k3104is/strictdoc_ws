#include "calc/calculator.hpp"
#include "calc/cli.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
namespace {
struct CliResult { int code; std::string output; std::string error; };
CliResult run(const std::vector<std::string_view>& args) {
  std::ostringstream output, error;
  int code = calc::run_cli(args, output, error);
  return {code, output.str(), error.str()};
}
TEST(CalculatorTest, SupportsAllOperations) {
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::add, 7, 2), 9);
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::sub, 7, 2), 5);
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::mul, 7, 2), 14);
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::div, 7, 2), 3.5);
}
TEST(CalculatorTest, RejectsDivisionByZero) { EXPECT_FALSE(calc::calculate(calc::Operation::div, 1, 0)); }
TEST(CliTest, PrintsCalculationResult) { auto r=run({"add","1.5","2.25"}); EXPECT_EQ(r.code,0); EXPECT_EQ(r.output,"3.75\n"); EXPECT_TRUE(r.error.empty()); }
TEST(CliTest, RejectsWrongArgumentCount) { auto r=run({"add","1"}); EXPECT_EQ(r.code,2); EXPECT_NE(r.error.find("Usage:"),std::string::npos); }
TEST(CliTest, RejectsUnknownOperation) { auto r=run({"pow","2","3"}); EXPECT_EQ(r.code,2); EXPECT_NE(r.error.find("unknown operation"),std::string::npos); }
TEST(CliTest, RejectsInvalidOperands) {
  for (std::string_view value : {"abc","1x","nan","inf","1e999"}) {
    auto r=run({"mul",value,"2"}); EXPECT_EQ(r.code,2)<<value; EXPECT_NE(r.error.find("finite numbers"),std::string::npos)<<value;
  }
}
TEST(CliTest, ReportsDivisionByZero) { auto r=run({"div","8","-0"}); EXPECT_EQ(r.code,1); EXPECT_NE(r.error.find("division by zero"),std::string::npos); }
TEST(CliTest, RejectsOverflowingResult) { auto r=run({"mul","1e308","10"}); EXPECT_EQ(r.code,1); EXPECT_NE(r.error.find("finite numeric range"),std::string::npos); }
}
