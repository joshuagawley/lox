// SPDX-License-Identifier: Apache-2.0

#include "chunk.h"

#include <iostream>

namespace {

std::size_t SimpleInstruction(std::string_view name, std::size_t offset) {
  std::cout << name << '\n';
  return offset + 1;
}

}  // namespace

namespace lox {

std::ostream &operator<<(std::ostream &os, Opcode opcode) {
  os << static_cast<uint8_t>(opcode);
  return os;
}

void Chunk::Write(std::uint8_t code, std::size_t line) noexcept {
  code_.push_back(code);
  lines_.push_back(line);
}

void Chunk::Write(Opcode code, std::size_t line) noexcept {
  code_.push_back(static_cast<std::uint8_t>(code));
  lines_.push_back(line);
}

void Chunk::Disassemble(std::string_view name) noexcept {
  std::cout << "== " << name << " ==\n";

  auto offset = std::size_t{0};
  while (offset < code_.size()) {
    offset = DisassembleInstruction(offset);
  }
}

void Chunk::WriteConstant(Value value, std::size_t line) noexcept {
  constants_.push_back(value);
  std::size_t index = constants_.size() - 1;
  if (index < 255) {
    Write(Opcode::kConstant, line);
    Write(static_cast<uint8_t>(constants_.size() - 1), line);
  } else {
    Write(Opcode::kConstantLong, line);
    Write((index & 0x00ff0000) >> 16, line);
    Write((index & 0x0000ff00) >> 8, line);
    Write(index & 0x000000ff, line);
  }
}

const std::uint8_t *Chunk::GetCodePtr() const noexcept { return code_.data(); }

std::size_t Chunk::GetLineAtIndex(std::size_t index) { return lines_[index]; }

Value Chunk::GetValueAtIndex(std::size_t index) { return constants_[index]; }

std::size_t Chunk::ConstantInstruction(std::string_view name,
                                       std::size_t offset) noexcept {
  std::uint8_t constant = code_[offset + 1];
  std::printf("%-16s %4d '", name.data(), constant);
  std::cout << constants_[constant] << "'\n";
  return offset + 2;
}

std::size_t Chunk::ConstantLongInstruction(std::string_view name,
                                           std::size_t offset) noexcept {
  auto constant =
      (code_[offset + 1] << 16) | (code_[offset + 2] << 8) | code_[offset + 3];
  std::printf("%-16s %4d '", name.data(), constant);
  std::cout << constants_[static_cast<std::size_t>(constant)] << "'\n";
  return offset + 4;
}

std::size_t Chunk::DisassembleInstruction(std::size_t offset) noexcept {
  std::printf("%04lu ", offset);

  if (offset > 0 && lines_[offset] == lines_[offset - 1]) {
    std::cout << "   | ";
  } else {
    std::printf("%4lu ", lines_[offset]);
  }

  auto instruction = static_cast<Opcode>(code_[offset]);
  switch (instruction) {
    case Opcode::kConstant:
      return ConstantInstruction("OP_CONSTANT", offset);
    case Opcode::kConstantLong:
      return ConstantLongInstruction("OP_CONSTANT_LONG", offset);
    case Opcode::kNil:
      return SimpleInstruction("OP_NIL", offset);
    case Opcode::kTrue:
      return SimpleInstruction("OP_TRUE", offset);
    case Opcode::kFalse:
      return SimpleInstruction("OP_FALSE", offset);
    case Opcode::kEqual:
      return SimpleInstruction("OP_EQUAL", offset);
    case Opcode::kNotEqual:
      return SimpleInstruction("OP_NOT_EQUAL", offset);
    case Opcode::kGreater:
      return SimpleInstruction("OP_GREATER", offset);
    case Opcode::kGreaterEqual:
      return SimpleInstruction("OP_GREATER_EQUAL", offset);
    case Opcode::kLess:
      return SimpleInstruction("OP_LESS", offset);
    case Opcode::kLessEqual:
      return SimpleInstruction("OP_LESS_EQUAL", offset);
    case Opcode::kAdd:
      return SimpleInstruction("OP_ADD", offset);
    case Opcode::kSubtract:
      return SimpleInstruction("OP_SUBTRACT", offset);
    case Opcode::kMultiply:
      return SimpleInstruction("OP_MULTIPLY", offset);
    case Opcode::kDivide:
      return SimpleInstruction("OP_DIVIDE", offset);
    case Opcode::kNot:
      return SimpleInstruction("OP_NOT", offset);
    case Opcode::kNegate:
      return SimpleInstruction("OP_NEGATE", offset);
    case Opcode::kReturn:
      return SimpleInstruction("OP_RETURN", offset);
    default:
      std::cout << "Unknown opcode " << instruction << '\n';
      return offset + 1;
  }
}

}  // namespace lox
