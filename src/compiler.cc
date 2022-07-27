// SPDX-License-Identifier: Apache-2.0

#include "compiler.h"

#define DEBUG_PRINT_CODE true

#include <iostream>

namespace {

constexpr auto kNoParseRule =
    lox::ParseRule{nullptr, nullptr, lox::Precedence::kNone};

}

namespace lox {

auto Compiler::Compile(std::string_view source, Chunk *chunk) -> bool {
  scanner_(source);
  current_ = scanner_.ScanToken();
  compiling_chunk_ = chunk;
  Expression();
  Consume(TokenType::kEof, "Expected end of expression");
  StopCompiling();
  return !had_error_;
}

auto Compiler::Advance() -> void {
  previous_ = current_.value();

  while (true) {
    current_ = scanner_.ScanToken();
    if (current_.value().type != TokenType::kError) break;

    ErrorAtCurrent(current_.value().lexeme);
  }
}

auto Compiler::Binary() -> void {
  auto operator_type = previous_.value().type;
  auto rule = GetParseRule(operator_type);
  ParsePrecedence(
      static_cast<Precedence>(static_cast<int>(rule.precedence) + 1));

  switch (operator_type) {
    case TokenType::kPlus:
      EmitByte(Opcode::kAdd);
      break;
    case TokenType::kMinus:
      EmitByte(Opcode::kSubtract);
      break;
    case TokenType::kStar:
      EmitByte(Opcode::kMultiply);
      break;
    case TokenType::kSlash:
      EmitByte(Opcode::kDivide);
      break;
    default:
      return;
  }
}

auto Compiler::Consume(TokenType type, std::string_view message) -> void {
  if (current_.value().type == type) {
    Advance();
    return;
  }

  ErrorAtCurrent(message);
}

auto Compiler::EmitByte(std::uint8_t byte) -> void {
  GetCurrentChunk()->Write(byte, previous_.value().line);
}

auto Compiler::EmitByte(Opcode code) -> void {
  GetCurrentChunk()->Write(code, previous_.value().line);
}

auto Compiler::EmitBytes(std::initializer_list<std::uint8_t> bytes) -> void {
  for (auto byte : bytes) {
    EmitByte(byte);
  }
}

auto Compiler::EmitBytes(std::initializer_list<Opcode> codes) -> void {
  for (auto code : codes) {
    EmitByte(code);
  }
}

auto Compiler::EmitReturn() -> void { EmitByte(Opcode::kReturn); }

auto Compiler::ErrorAt(const Token &token, std::string_view message) -> void {
  if (panic_mode_) return;
  panic_mode_ = true;
  std::cerr << "[line " << token.line << " Error";

  if (token.type == lox::TokenType::kEof) {
    std::cerr << " at end";
  } else if (token.type == lox::TokenType::kError) {
  } else {
    std::cerr << " at '" << token.lexeme << "'";
  }

  std::cerr << ": " << message << '\n';
  had_error_ = true;
}

auto Compiler::Error(std::string_view message) -> void {
  ErrorAt(previous_.value(), message);
}

auto Compiler::ErrorAtCurrent(std::string_view message) -> void {
  ErrorAt(current_.value(), message);
}

auto Compiler::Expression() -> void {
  ParsePrecedence(Precedence::kAssignment);
}

auto Compiler::GetCurrentChunk() -> Chunk * { return compiling_chunk_; }

constexpr auto Compiler::GetParseRule(TokenType type) -> ParseRule {
  switch (type) {
    case TokenType::kLeftParen:
      return {&Compiler::Grouping, nullptr, Precedence::kNone};
    case TokenType::kMinus:
      return {&Compiler::Unary, &Compiler::Binary, Precedence::kTerm};
    case TokenType::kPlus:
      return {nullptr, &Compiler::Binary, Precedence::kTerm};
    case TokenType::kSlash:
    case TokenType::kStar:
      return {nullptr, &Compiler::Binary, Precedence::kFactor};
    case TokenType::kNumber:
      return {&Compiler::Number, nullptr, Precedence::kNone};
    default:
      return kNoParseRule;
  }
}

auto Compiler::Grouping() -> void {
  Expression();
  Consume(TokenType::kRightParen, "Expected ')' after expression");
}

auto Compiler::Number() -> void {
  auto value = std::stod(previous_.value().lexeme.data());
  GetCurrentChunk()->WriteConstant(value, previous_.value().line);
}

auto Compiler::ParsePrecedence(Precedence precedence) -> void {
  Advance();
  auto prefixRule = GetParseRule(previous_.value().type).prefix_func;
  if (prefixRule == nullptr) {
    Error("Expected expression.");
    return;
  }

  (this->*prefixRule)();

  while (precedence <= GetParseRule(current_.value().type).precedence) {
    Advance();
    auto infix_rule = GetParseRule(previous_.value().type).infix_func;
    (this->*infix_rule)();
  }
}

auto Compiler::StopCompiling() -> void {
  EmitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!had_error_) {
    GetCurrentChunk()->Disassemble("code");
  }
#endif
}

auto Compiler::Unary() -> void {
  auto operator_type = previous_.value().type;

  // Compile the operand
  ParsePrecedence(Precedence::kUnary);

  switch (operator_type) {
    case TokenType::kMinus:
      EmitByte(Opcode::kNegate);
    default:
      return;
  }
}

}  // namespace lox
