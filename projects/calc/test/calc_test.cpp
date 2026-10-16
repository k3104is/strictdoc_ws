#include "calc/calculator.hpp"
#include "calc/cli.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

struct CliResult {
  int code;
  std::string output;
  std::string error;
};

CliResult run(const std::vector<std::string_view>& args) {
  std::ostringstream output;
  std::ostringstream error;
  const int code = calc::run_cli(args, output, error);
  return {code, output.str(), error.str()};
}

std::string read_file(const std::filesystem::path& path) {
  std::ifstream input(path);
  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}

class CliTest : public ::testing::Test {
 protected:
  void SetUp() override {
    original_path_ = std::filesystem::current_path();
    const auto* test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    test_path_ = std::filesystem::temp_directory_path() /
                 ("calc-test-" + std::string(test_info->name()));
    std::filesystem::remove_all(test_path_);
    std::filesystem::create_directories(test_path_ / "log");
    std::filesystem::current_path(test_path_);
  }

  void TearDown() override {
    std::filesystem::current_path(original_path_);
    std::filesystem::remove_all(test_path_);
  }

  std::filesystem::path only_log_file() const {
    std::filesystem::path result;
    for (const auto& entry : std::filesystem::directory_iterator("log")) {
      if (entry.is_regular_file()) {
        EXPECT_TRUE(result.empty());
        result = entry.path();
      }
    }
    EXPECT_FALSE(result.empty());
    return result;
  }

 private:
  std::filesystem::path original_path_;
  std::filesystem::path test_path_;
};

TEST(CalculatorTest, SupportsAllOperations) {
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::add, 7, 2), 9);
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::sub, 7, 2), 5);
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::mul, 7, 2), 14);
  EXPECT_DOUBLE_EQ(*calc::calculate(calc::Operation::div, 7, 2), 3.5);
}

TEST(CalculatorTest, RejectsDivisionByZero) {
  EXPECT_FALSE(calc::calculate(calc::Operation::div, 1, 0));
}

TEST_F(CliTest, PrintsCalculationResult) {
  const auto result = run({"add", "1.5", "2.25"});
  EXPECT_EQ(result.code, 0);
  EXPECT_EQ(result.output, "3.75\n");
  EXPECT_TRUE(result.error.empty());
}

TEST_F(CliTest, RejectsWrongArgumentCount) {
  const auto result = run({"add", "1"});
  EXPECT_EQ(result.code, 2);
  EXPECT_NE(result.error.find("Usage:"), std::string::npos);
}

TEST_F(CliTest, RejectsUnknownOperation) {
  const auto result = run({"pow", "2", "3"});
  EXPECT_EQ(result.code, 2);
  EXPECT_NE(result.error.find("unknown operation"), std::string::npos);
}

TEST_F(CliTest, RejectsInvalidOperands) {
  for (std::string_view value : {"abc", "1x", "nan", "inf", "1e999"}) {
    const auto result = run({"mul", value, "2"});
    EXPECT_EQ(result.code, 2) << value;
    EXPECT_NE(result.error.find("finite numbers"), std::string::npos) << value;
  }
}

TEST_F(CliTest, ReportsDivisionByZero) {
  const auto result = run({"div", "8", "-0"});
  EXPECT_EQ(result.code, 1);
  EXPECT_NE(result.error.find("division by zero"), std::string::npos);
}

TEST_F(CliTest, RejectsOverflowingResult) {
  const auto result = run({"mul", "1e308", "10"});
  EXPECT_EQ(result.code, 1);
  EXPECT_NE(result.error.find("finite numeric range"), std::string::npos);
}

TEST_F(CliTest, AppendsSuccessfulCalculationsToDailyLog) {
  EXPECT_EQ(run({"add", "1", "2"}).code, 0);
  EXPECT_EQ(run({"mul", "3", "4"}).code, 0);

  const std::string log = read_file(only_log_file());
  EXPECT_TRUE(std::regex_match(
      log,
      std::regex(
          R"((\d{2}:\d{2}:\d{2} \| 1 \+ 2 \| 3\n)(\d{2}:\d{2}:\d{2} \| 3 \* 4 \| 12\n))")));
}

TEST_F(CliTest, WritesErrorsToDailyLog) {
  const auto result = run({"div", "8", "0"});
  ASSERT_EQ(result.code, 1);

  const std::string log = read_file(only_log_file());
  EXPECT_TRUE(std::regex_match(
      log,
      std::regex(
          R"(\d{2}:\d{2}:\d{2} \| ERROR \| Error: division by zero\n)")));
}

TEST_F(CliTest, ShowsLogForSpecifiedDate) {
  std::ofstream("log/2026-10-16.log")
      << "12:34:56 | 1 + 2 | 3\n";
  const auto result = run({"log", "2026-10-16"});
  EXPECT_EQ(result.code, 0);
  EXPECT_EQ(result.output, "12:34:56 | 1 + 2 | 3\n");
  EXPECT_TRUE(result.error.empty());
}

TEST_F(CliTest, RejectsInvalidLogDate) {
  const auto result = run({"log", "2026-02-30"});
  EXPECT_NE(result.code, 0);
  EXPECT_NE(result.output.find("YYYY-MM-DD"), std::string::npos);
}

TEST_F(CliTest, ReportsMissingLogDirectoryOnStandardOutput) {
  std::filesystem::remove_all("log");
  const auto result = run({"add", "1", "2"});
  EXPECT_NE(result.code, 0);
  EXPECT_EQ(result.output, "Error: log directory does not exist\n");
  EXPECT_TRUE(result.error.empty());
}

TEST_F(CliTest, ReportsLogWriteFailureAndSuppressesResult) {
  const auto first = run({"add", "1", "2"});
  ASSERT_EQ(first.code, 0);
  const auto path = only_log_file();
  std::filesystem::remove(path);
  std::filesystem::create_directory(path);

  const auto result = run({"add", "2", "3"});
  EXPECT_NE(result.code, 0);
  EXPECT_EQ(result.output, "Error: unable to write log file\n");
  EXPECT_TRUE(result.error.empty());
}

TEST_F(CliTest, ReportsLogReadFailure) {
  const auto result = run({"log", "2026-10-16"});
  EXPECT_NE(result.code, 0);
  EXPECT_EQ(result.output, "Error: unable to read log file\n");
  EXPECT_TRUE(result.error.empty());
}

}  // namespace
