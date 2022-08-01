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
  bool Compile(std::string_view source, Chunk *chunk);

 private:
  void Advance();
  void Binary();
  void Consume(TokenType type, std::string_view message);
  void EmitByte(std::uint8_t byte);
  void EmitByte(Opcode code);
  void EmitBytes(std::initializer_list<std::uint8_t> bytes);
  void EmitBytes(std::initializer_list<Opcode> codes);
  void EmitReturn();
  void ErrorAt(const Token &token, std::string_view message);
  void ErrorAtCurrent(std::string_view message);
  void Error(std::string_view message);
  void Expression();
  Chunk *GetCurrentChunk();
  static constexpr ParseRule GetParseRule(TokenType type);
  void Grouping();
  void Number();
  void ParsePrecedence(Precedence precedence);
  void StopCompiling();
  void Unary();

  Chunk *compiling_chunk_ = nullptr;
  std::optional<Token> current_;
  std::optional<Token> previous_;
  bool had_error_ = false;
  bool panic_mode_ = false;
  Scanner scanner_;
};

}  // namespace lox

#endif  // LOX_SRC_COMPILER_H
