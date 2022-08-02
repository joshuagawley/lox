// SPDX-License-Identifier: Apache-2.0

#include <sysexits.h>

#include <fstream>
#include <iostream>
#include <string>

#include "vm.h"

namespace lox {

// From
// https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
std::string ReadFile(std::string_view path) {
  constexpr auto read_size = std::size_t{4096};
  auto stream = std::ifstream{path.data()};
  stream.exceptions(std::ios::badbit);

  auto result = std::string{};
  auto buffer = std::string(read_size, '\0');
  while (stream.read(buffer.data(), read_size)) {
    result.append(buffer, 0, static_cast<std::size_t>(stream.gcount()));
  }
  result.append(buffer, 0, static_cast<std::size_t>(stream.gcount()));
  return result;
}

void Repl() {
  auto vm = VirtualMachine{};
  auto line = std::string{};

  while (std::cout << "> " && std::getline(std::cin, line)) {
    vm.Interpret(line);
    std::cout << '\n';
  }
}

int RunFile(std::string_view path) {
  auto vm = VirtualMachine{};
  std::string source = ReadFile(path);
  InterpretResult result = vm.Interpret(source);

  switch (result) {
    case InterpretResult::kOk:
      return EXIT_SUCCESS;
    case InterpretResult::kCompileError:
      return EX_DATAERR;
    case InterpretResult::kRuntimeError:
      return EX_SOFTWARE;
  }
}

}  // namespace lox

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    lox::Repl();
  } else if (argc == 2) {
    return lox::RunFile(argv[1]);
  } else {
    std::cerr << "Usage: lox [path]\n";
    return EX_USAGE;
  }

  return EXIT_SUCCESS;
}
