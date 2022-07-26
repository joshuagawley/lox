// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_CHUNK_H
#define LOX_SRC_CHUNK_H

#include <cstdint>
#include <string_view>
#include <vector>

#include "value.h"

namespace lox {

enum class Opcode : std::uint8_t { kConstant, kConstantLong, kReturn };

std::ostream &operator<<(std::ostream &os, Opcode opcode);

class Chunk {
 public:
  Chunk() noexcept = default;
  ~Chunk() noexcept = default;

  auto Disassemble(std::string_view name) noexcept -> void;
  auto Write(Opcode code, std::size_t line) noexcept -> void;
  auto Write(std::uint8_t code, std::size_t line) noexcept -> void;
  auto WriteConstant(Value value, std::size_t line) noexcept -> void;

 private:
  auto ConstantInstruction(std::string_view name, std::size_t offset) noexcept
      -> std::size_t;
  auto ConstantLongInstruction(std::string_view name,
                               std::size_t offset) noexcept -> std::size_t;
  auto DisassembleInstruction(std::size_t offset) noexcept -> std::size_t;

  std::vector<std::uint8_t> code_;
  std::vector<std::size_t> lines_;
  std::vector<Value> constants_;
};

} // namespace lox

#endif  // LOX_SRC_CHUNK_H