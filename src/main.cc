// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "chunk.h"

int main(int argc, const char *argv[]) {
  auto chunk = lox::Chunk{};
  for (auto i = 0; i < 300; ++i) {
    chunk.WriteConstant(1.2, 123);
  }
  chunk.Write(lox::Opcode::kReturn, 123);
  chunk.Disassemble("test chunk");
  return EXIT_SUCCESS;
}