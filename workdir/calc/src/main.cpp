#include "calc/cli.hpp"
#include <iostream>
#include <string_view>
#include <vector>
int main(int argc, char* argv[]) {
  std::vector<std::string_view> arguments;
  arguments.reserve(argc > 0 ? static_cast<std::size_t>(argc - 1) : 0);
  for (int i = 1; i < argc; ++i) arguments.emplace_back(argv[i]);
  return calc::run_cli(arguments, std::cout, std::cerr);
}
