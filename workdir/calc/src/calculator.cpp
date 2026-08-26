#include "calc/calculator.hpp"
#include <cmath>
namespace calc {
std::optional<Operation> parse_operation(std::string_view name) {
  if (name == "add") return Operation::add;
  if (name == "sub") return Operation::sub;
  if (name == "mul") return Operation::mul;
  if (name == "div") return Operation::div;
  return std::nullopt;
}
std::optional<double> calculate(Operation operation, double lhs, double rhs) {
  if (!std::isfinite(lhs) || !std::isfinite(rhs)) return std::nullopt;
  double result = 0;
  switch (operation) {
    case Operation::add: result = lhs + rhs; break;
    case Operation::sub: result = lhs - rhs; break;
    case Operation::mul: result = lhs * rhs; break;
    case Operation::div: if (rhs == 0) return std::nullopt; result = lhs / rhs; break;
  }
  return std::isfinite(result) ? std::optional<double>{result} : std::nullopt;
}
}
