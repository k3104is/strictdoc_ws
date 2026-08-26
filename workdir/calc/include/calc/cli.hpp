#pragma once
#include <iosfwd>
#include <string_view>
#include <vector>
namespace calc {
int run_cli(const std::vector<std::string_view>& arguments, std::ostream& output, std::ostream& error);
}
