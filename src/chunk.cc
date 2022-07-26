// SPDX-License-Identifier: Apache-2.0

#include "chunk.h"

#include <iostream>

namespace {

constexpr auto SimpleInstruction(std::string_view name, std::size_t offset)
    -> std::size_t {
  std::cout << name << '\n';
  return offset + 1;
}

}  // namespace

namespace lox {

std::ostream &operator<<(std::ostream &os, Opcode opcode) {
  os << static_cast<uint8_t>(opcode);
  return os;
}

auto Chunk::Write(std::uint8_t code, std::size_t line) noexcept -> void {
  code_.push_back(code);
  lines_.push_back(line);
}

auto Chunk::Write(Opcode code, std::size_t line) noexcept -> void {
  code_.push_back(static_cast<std::uint8_t>(code));
  lines_.push_back(line);
}

auto Chunk::Disassemble(std::string_view name) noexcept -> void {
  std::cout << "== " << name << " ==\n";

  auto offset = std::size_t{0};
  while (offset < code_.size()) {
    offset = DisassembleInstruction(offset);
  }
}

auto Chunk::WriteConstant(Value value, std::size_t line) noexcept -> void {
  constants_.push_back(value);
  auto index = constants_.size() - 1;
  if (index < 255) {
    Write(Opcode::kConstant, line);
    Write(constants_.size() - 1, line);
  } else {
    Write(Opcode::kConstantLong, line);
    Write((index & 0x00ff0000) >> 16, line);
    Write((index & 0x0000ff00) >> 8, line);
    Write(index & 0x000000ff, line);
  }
}

auto Chunk::ConstantInstruction(std::string_view name,
                                std::size_t offset) noexcept -> std::size_t {
  auto constant = code_[offset + 1];
  std::printf("%-16s %4d '", name.data(), constant);
  std::cout << constants_[constant] << "'\n";
  return offset + 2;
}

auto Chunk::ConstantLongInstruction(std::string_view name,
                                    std::size_t offset) noexcept
    -> std::size_t {
  auto constant =
      (code_[offset + 1] << 16) | (code_[offset + 2] << 8) | code_[offset + 3];
  std::printf("%-16s %4d '", name.data(), constant);
  std::cout << constants_[constant] << "'\n";
  return offset + 4;
}

auto Chunk::DisassembleInstruction(std::size_t offset) noexcept -> std::size_t {
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
    case Opcode::kReturn:
      return SimpleInstruction("OP_RETURN", offset);
    default:
      std::cout << "Unknown opcode " << instruction << '\n';
      return offset + 1;
  }
}

}  // namespace lox