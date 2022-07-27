// SPDX-License-Identifier: Apache-2.0

#include "compiler.h"

#include <cstdio>
#include <iostream>

auto lox::Compiler::Compile(std::string_view source) -> void {
  scanner_(source);
  auto line = static_cast<std::size_t>(-1);
  while (true) {
    auto token = scanner_.ScanToken();
    if (token.line != line) {
      std::printf("%4lu ", token.line);
      line = token.line;
    } else {
      std::cout << "   | ";
    }
    std::printf("%2d '%s\n", token.type, token.lexeme.data());

    if (token.type == TokenType::kEof) break;
  }
}
