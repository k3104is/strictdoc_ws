#include "calc/cli.hpp"
#include "calc/calculator.hpp"
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <optional>
#include <ostream>
#include <string>
namespace calc {
namespace {
std::optional<double> parse_number(std::string_view text) {
  const std::string value(text);
  char* end = nullptr;
  errno = 0;
  const double number = std::strtod(value.c_str(), &end);
  if (value.empty() || end != value.c_str() + value.size() || errno == ERANGE || !std::isfinite(number)) return std::nullopt;
  return number;
}
}
int run_cli(const std::vector<std::string_view>& arguments, std::ostream& output, std::ostream& error) {
  if (arguments.size() != 3) { error << "Usage: calc <add|sub|mul|div> <lhs> <rhs>\n"; return 2; }
  const auto operation = parse_operation(arguments[0]);
  if (!operation) { error << "Error: unknown operation: " << arguments[0] << '\n'; return 2; }
  const auto lhs = parse_number(arguments[1]);
  const auto rhs = parse_number(arguments[2]);
  if (!lhs || !rhs) { error << "Error: operands must be finite numbers\n"; return 2; }
  if (*operation == Operation::div && *rhs == 0) { error << "Error: division by zero\n"; return 1; }
  const auto result = calculate(*operation, *lhs, *rhs);
  if (!result) { error << "Error: result is outside the finite numeric range\n"; return 1; }
  output << std::setprecision(15) << *result << '\n';
  return 0;
}
}
