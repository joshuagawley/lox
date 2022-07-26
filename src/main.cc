// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "chunk.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
  auto vm = lox::VirtualMachine{};

  auto chunk = lox::Chunk{};
  chunk.WriteConstant(1.2, 123);
  chunk.WriteConstant(3.4, 123);
  chunk.Write(lox::Opcode::kAdd, 123);
  chunk.WriteConstant(5.6, 123);
  chunk.Write(lox::Opcode::kDivide, 123);
  chunk.Write(lox::Opcode::kNegate, 123);
  chunk.Write(lox::Opcode::kReturn, 123);
  chunk.Disassemble("test chunk");
  vm.Interpret(&chunk);
  return EXIT_SUCCESS;
}