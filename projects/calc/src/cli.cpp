#include "calc/cli.hpp"
#include "calc/calculator.hpp"

#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

namespace calc {
namespace {

struct LocalDateTime {
  std::string date;
  std::string time;
};

std::optional<double> parse_number(std::string_view text) {
  const std::string value(text);
  char* end = nullptr;
  errno = 0;
  const double number = std::strtod(value.c_str(), &end);
  if (value.empty() || end != value.c_str() + value.size() ||
      errno == ERANGE || !std::isfinite(number)) {
    return std::nullopt;
  }
  return number;
}

LocalDateTime local_date_time() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t value = std::chrono::system_clock::to_time_t(now);
  std::tm local{};
#ifdef _WIN32
  localtime_s(&local, &value);
#else
  localtime_r(&value, &local);
#endif

  std::ostringstream date;
  date << std::put_time(&local, "%Y-%m-%d");
  std::ostringstream time;
  time << std::put_time(&local, "%H:%M:%S");
  return {date.str(), time.str()};
}

bool valid_date(std::string_view text) {
  if (text.size() != 10 || text[4] != '-' || text[7] != '-') return false;
  for (std::size_t i = 0; i < text.size(); ++i) {
    if (i != 4 && i != 7 && (text[i] < '0' || text[i] > '9')) return false;
  }

  const int year = std::stoi(std::string(text.substr(0, 4)));
  const int month = std::stoi(std::string(text.substr(5, 2)));
  const int day = std::stoi(std::string(text.substr(8, 2)));
  if (month < 1 || month > 12) return false;
  static constexpr int days_per_month[] =
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int maximum_day = days_per_month[month - 1];
  const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
  if (month == 2 && leap) maximum_day = 29;
  return day >= 1 && day <= maximum_day;
}

std::string operation_symbol(Operation operation) {
  switch (operation) {
    case Operation::add: return "+";
    case Operation::sub: return "-";
    case Operation::mul: return "*";
    case Operation::div: return "/";
  }
  return "";
}

std::filesystem::path log_path(std::string_view date) {
  return std::filesystem::path("log") / (std::string(date) + ".log");
}

bool log_directory_exists() {
  std::error_code error;
  return std::filesystem::is_directory("log", error) && !error;
}

bool append_log(std::string_view line, std::ostream& output) {
  if (!log_directory_exists()) {
    output << "Error: log directory does not exist\n";
    return false;
  }

  const auto date_time = local_date_time();
  std::ofstream log(log_path(date_time.date), std::ios::app);
  if (!log || !(log << date_time.time << " | " << line << '\n')) {
    output << "Error: unable to write log file\n";
    return false;
  }
  return true;
}

int report_calculation_error(std::string_view message, int code,
                             std::ostream& output, std::ostream& error) {
  if (!append_log(std::string("ERROR | ") + std::string(message), output)) {
    return 1;
  }
  error << message << '\n';
  return code;
}

int show_log(std::string_view date, std::ostream& output) {
  if (!valid_date(date)) {
    output << "Error: log date must use YYYY-MM-DD\n";
    return 1;
  }
  if (!log_directory_exists()) {
    output << "Error: log directory does not exist\n";
    return 1;
  }

  std::ifstream log(log_path(date));
  if (!log) {
    output << "Error: unable to read log file\n";
    return 1;
  }
  output << log.rdbuf();
  if (log.bad()) {
    output << "Error: unable to read log file\n";
    return 1;
  }
  return 0;
}

}  // namespace

int run_cli(const std::vector<std::string_view>& arguments,
            std::ostream& output, std::ostream& error) {
  if (!arguments.empty() && arguments[0] == "log") {
    if (arguments.size() != 2) {
      output << "Usage: calc log YYYY-MM-DD\n";
      return 1;
    }
    return show_log(arguments[1], output);
  }

  if (arguments.size() != 3) {
    return report_calculation_error(
        "Usage: calc <add|sub|mul|div> <lhs> <rhs>", 2, output, error);
  }

  const auto operation = parse_operation(arguments[0]);
  if (!operation) {
    return report_calculation_error(
        std::string("Error: unknown operation: ") + std::string(arguments[0]),
        2, output, error);
  }

  const auto lhs = parse_number(arguments[1]);
  const auto rhs = parse_number(arguments[2]);
  if (!lhs || !rhs) {
    return report_calculation_error(
        "Error: operands must be finite numbers", 2, output, error);
  }
  if (*operation == Operation::div && *rhs == 0) {
    return report_calculation_error(
        "Error: division by zero", 1, output, error);
  }

  const auto result = calculate(*operation, *lhs, *rhs);
  if (!result) {
    return report_calculation_error(
        "Error: result is outside the finite numeric range", 1, output, error);
  }

  std::ostringstream result_text;
  result_text << std::setprecision(15) << *result;
  const std::string calculation =
      std::string(arguments[1]) + " " + operation_symbol(*operation) + " " +
      std::string(arguments[2]) + " | " + result_text.str();
  if (!append_log(calculation, output)) return 1;

  output << result_text.str() << '\n';
  return 0;
}

}  // namespace calc
