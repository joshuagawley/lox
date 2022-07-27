// SPDX-License-Identifier: Apache-2.0

#include <fstream>
#include <iostream>

#include "chunk.h"
#include "vm.h"

namespace lox {

// From
// https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
auto ReadFile(std::string_view path) -> std::string {
  constexpr auto read_size = std::size_t{4096};
  auto stream = std::ifstream{path.data()};
  stream.exceptions(std::ios::badbit);

  auto result = std::string{};
  auto buffer = std::string(read_size, '\0');
  while (stream.read(&buffer[0], read_size)) {
    result.append(buffer, 0, stream.gcount());
  }
  result.append(buffer, 0, stream.gcount());
  return result;
}

auto Repl() -> void {
  auto vm = lox::VirtualMachine{};
  auto line = std::string{};

  while (std::cout << "> " && std::getline(std::cin, line)) {
    vm.Interpret(line);
  }
  std::cout << '\n';
}

auto RunFile(std::string_view path) -> void {
  auto vm = VirtualMachine{};
  auto source = ReadFile(path);
  auto result = vm.Interpret(source);

  if (result == InterpretResult::kCompileError) {
    std::exit(65);
  }
  if (result == InterpretResult::kRuntimeError) {
    std::exit(70);
  }
}

}  // namespace lox

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    lox::Repl();
  } else if (argc == 2) {
    lox::RunFile(argv[1]);
  } else {
    std::cerr << "Usage: lox [path]\n";
    std::exit(64);
  }

  return EXIT_SUCCESS;
}