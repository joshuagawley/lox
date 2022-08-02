// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_CHUNK_H
#define LOX_SRC_CHUNK_H

#include <cstdint>
#include <string_view>
#include <vector>

#include "value.h"

namespace lox {

enum class Opcode : std::uint8_t {
  kConstant,
  kConstantLong,
  kNil,
  kTrue,
  kFalse,
  kEqual,
  kNotEqual,
  kGreater,
  kGreaterEqual,
  kLess,
  kLessEqual,
  kAdd,
  kSubtract,
  kMultiply,
  kDivide,
  kNot,
  kNegate,
  kReturn
};

std::ostream &operator<<(std::ostream &os, Opcode opcode);

class Chunk {
 public:
  Chunk() noexcept = default;
  Chunk(const Chunk &) = delete;
  Chunk(Chunk &&) = delete;
  void operator=(const Chunk &) = delete;
  void operator=(Chunk &&) = delete;
  ~Chunk() noexcept = default;

  void Disassemble(std::string_view name) noexcept;
  void Write(Opcode code, std::size_t line) noexcept;
  void Write(std::uint8_t code, std::size_t line) noexcept;
  void WriteConstant(Value value, std::size_t line) noexcept;
  [[nodiscard]] const std::uint8_t *GetCodePtr() const noexcept;
  std::size_t GetLineAtIndex(std::size_t index);
  Value GetValueAtIndex(std::size_t index);
  std::size_t DisassembleInstruction(std::size_t offset) noexcept;

 private:
  std::size_t ConstantInstruction(std::string_view name,
                                  std::size_t offset) noexcept;
  std::size_t ConstantLongInstruction(std::string_view name,
                                      std::size_t offset) noexcept;

  std::vector<std::uint8_t> code_;
  std::vector<std::size_t> lines_;
  std::vector<Value> constants_;
};

}  // namespace lox

#endif  // LOX_SRC_CHUNK_H
