// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_COMPILER_H
#define LOX_SRC_COMPILER_H

#include <functional>
#include <optional>
#include <string_view>

#include "chunk.h"
#include "scanner.h"

namespace lox {

class Compiler;

using ParseFunc = void (Compiler::*)();

enum class Precedence {
  kNone,
  kAssignment,
  kOr,
  kAnd,
  kEquality,
  kComparison,
  kTerm,
  kFactor,
  kUnary,
  kCall,
  kPrimary
};

struct ParseRule {
  ParseFunc prefix_func;
  ParseFunc infix_func;
  Precedence precedence;
};

class Compiler {
 public:
  auto Compile(std::string_view source, Chunk *chunk) -> bool;

 private:
  auto Advance() -> void;
  auto Binary() -> void;
  auto Consume(TokenType type, std::string_view message) -> void;
  auto EmitByte(std::uint8_t byte) -> void;
  auto EmitByte(Opcode code) -> void;
  auto EmitBytes(std::initializer_list<std::uint8_t> bytes) -> void;
  auto EmitBytes(std::initializer_list<Opcode> codes) -> void;
  auto EmitReturn() -> void;
  auto ErrorAt(const Token &token, std::string_view message) -> void;
  auto ErrorAtCurrent(std::string_view message) -> void;
  auto Error(std::string_view message) -> void;
  auto Expression() -> void;
  auto GetCurrentChunk() -> Chunk *;
  static constexpr auto GetParseRule(TokenType type) -> ParseRule;
  auto Grouping() -> void;
  auto Number() -> void;
  auto ParsePrecedence(Precedence precedence) -> void;
  auto StopCompiling() -> void;
  auto Unary() -> void;

  Chunk *compiling_chunk_ = nullptr;
  std::optional<Token> current_;
  std::optional<Token> previous_;
  bool had_error_ = false;
  bool panic_mode_ = false;
  Scanner scanner_;
};

}  // namespace lox

#endif  // LOX_SRC_COMPILER_H
