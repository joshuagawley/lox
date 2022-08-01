// SPDX-License-Identifier: Apache-2.0

#include "compiler.h"

#define DEBUG_PRINT_CODE true

#include <iostream>

namespace {

constexpr auto kNoParseRule =
    lox::ParseRule{nullptr, nullptr, lox::Precedence::kNone};

}

namespace lox {

bool Compiler::Compile(std::string_view source, Chunk *chunk) {
  scanner_(source);
  current_ = scanner_.ScanToken();
  compiling_chunk_ = chunk;
  Expression();
  Consume(TokenType::kEof, "Expected end of expression");
  StopCompiling();
  return !had_error_;
}

void Compiler::Advance() {
  previous_ = current_.value();

  while (true) {
    current_ = scanner_.ScanToken();
    if (current_.value().type != TokenType::kError) break;

    ErrorAtCurrent(current_.value().lexeme);
  }
}

void Compiler::Binary() {
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

void Compiler::Consume(TokenType type, std::string_view message) {
  if (current_.value().type == type) {
    Advance();
    return;
  }

  ErrorAtCurrent(message);
}

void Compiler::EmitByte(std::uint8_t byte) {
  GetCurrentChunk()->Write(byte, previous_.value().line);
}

void Compiler::EmitByte(Opcode code) {
  GetCurrentChunk()->Write(code, previous_.value().line);
}

void Compiler::EmitBytes(std::initializer_list<std::uint8_t> bytes) {
  for (auto byte : bytes) {
    EmitByte(byte);
  }
}

void Compiler::EmitBytes(std::initializer_list<Opcode> codes) {
  for (auto code : codes) {
    EmitByte(code);
  }
}

void Compiler::EmitReturn() { EmitByte(Opcode::kReturn); }

void Compiler::ErrorAt(const Token &token, std::string_view message) {
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

void Compiler::Error(std::string_view message) {
  ErrorAt(previous_.value(), message);
}

void Compiler::ErrorAtCurrent(std::string_view message) {
  ErrorAt(current_.value(), message);
}

void Compiler::Expression() { ParsePrecedence(Precedence::kAssignment); }

Chunk *Compiler::GetCurrentChunk() { return compiling_chunk_; }

constexpr ParseRule Compiler::GetParseRule(TokenType type) {
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

void Compiler::Grouping() {
  Expression();
  Consume(TokenType::kRightParen, "Expected ')' after expression");
}

void Compiler::Number() {
  auto value = std::stod(previous_.value().lexeme.data());
  GetCurrentChunk()->WriteConstant(value, previous_.value().line);
}

void Compiler::ParsePrecedence(Precedence precedence) {
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

void Compiler::StopCompiling() {
  EmitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!had_error_) {
    GetCurrentChunk()->Disassemble("code");
  }
#endif
}

void Compiler::Unary() {
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
