#pragma once
#include <optional>
#include <string_view>
namespace calc {
enum class Operation { add, sub, mul, div };
std::optional<Operation> parse_operation(std::string_view name);
std::optional<double> calculate(Operation operation, double lhs, double rhs);
}
