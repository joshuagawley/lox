// SPDX-License-Identifier: Apache-2.0

#include "compiler.h"

#include <iostream>

#define DEBUG_PRINT_CODE

namespace {

constexpr auto kNoParseRule =
    lox::ParseRule{nullptr, nullptr, lox::Precedence::kNone};

}

namespace lox {

Compiler::Compiler(std::string_view source) : parser_(source) {}

bool Compiler::Compile(Chunk *chunk) {
  compiling_chunk_ = chunk;
  Expression();
  parser_.Consume(TokenType::kEof, "Expected end of expression");
  StopCompiling();
  return !parser_.had_error();
}

void Compiler::Binary() {
  auto operator_type = parser_.get_previous().type;
  auto rule = GetParseRule(operator_type);
  ParsePrecedence(
      static_cast<Precedence>(static_cast<int>(rule.precedence) + 1));

  switch (operator_type) {
    case TokenType::kBangEqual:
      EmitByte(Opcode::kNotEqual);
      break;
    case TokenType::kEqualEqual:
      EmitByte(Opcode::kEqual);
      break;
    case TokenType::kGreater:
      EmitByte(Opcode::kGreater);
      break;
    case TokenType::kGreaterEqual:
      EmitByte(Opcode::kGreaterEqual);
      break;
    case TokenType::kLess:
      EmitByte(Opcode::kLess);
      break;
    case TokenType::kLessEqual:
      EmitByte(Opcode::kLessEqual);
      break;
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

void Compiler::EmitByte(std::uint8_t byte) {
  GetCurrentChunk()->Write(byte, parser_.get_previous().line);
}

void Compiler::EmitByte(Opcode code) {
  GetCurrentChunk()->Write(code, parser_.get_previous().line);
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

void Compiler::Expression() { ParsePrecedence(Precedence::kAssignment); }

Chunk *Compiler::GetCurrentChunk() { return compiling_chunk_; }

constexpr ParseRule Compiler::GetParseRule(TokenType type) {
  switch (type) {
    case TokenType::kBang:
      return {&Compiler::Unary, nullptr, Precedence::kNone};
    case TokenType::kBangEqual:
    case TokenType::kEqualEqual:
      return {nullptr, &Compiler::Binary, Precedence::kEquality};
    case TokenType::kGreater:
    case TokenType::kGreaterEqual:
    case TokenType::kLess:
    case TokenType::kLessEqual:
      return {nullptr, &Compiler::Binary, Precedence::kComparison};
    case TokenType::kFalse:
    case TokenType::kTrue:
    case TokenType::kNil:
      return {&Compiler::Literal, nullptr, Precedence::kNone};
    case TokenType::kLeftParen:
      return {&Compiler::Grouping, nullptr, Precedence::kNone};
    case TokenType::kMinus:
      return {&Compiler::Unary, &Compiler::Binary, Precedence::kTerm};
    case TokenType::kPlus:
      return {nullptr, &Compiler::Binary, Precedence::kTerm};
    case TokenType::kSlash:
    case TokenType::kStar:
      return {nullptr, &Compiler::Binary, Precedence::kFactor};
    case TokenType::kString:
      return {&Compiler::String, nullptr, Precedence::kNone};
    case TokenType::kNumber:
      return {&Compiler::Number, nullptr, Precedence::kNone};
    default:
      return kNoParseRule;
  }
}

void Compiler::Grouping() {
  Expression();
  parser_.Consume(TokenType::kRightParen, "Expected ')' after expression");
}

void Compiler::Number() {
  auto value = std::stod(parser_.get_previous().lexeme.data());
  GetCurrentChunk()->WriteConstant(value, parser_.get_previous().line);
}

void Compiler::Literal() {
  switch (parser_.get_previous().type) {
    case TokenType::kFalse:
      EmitByte(Opcode::kFalse);
      break;
    case TokenType::kTrue:
      EmitByte(Opcode::kTrue);
      break;
    case TokenType::kNil:
      EmitByte(Opcode::kNil);
      break;
    default:
      return;
  }
}

void Compiler::ParsePrecedence(Precedence precedence) {
  parser_.Advance();
  auto prefixRule = GetParseRule(parser_.get_previous().type).prefix_func;
  if (prefixRule == nullptr) {
    parser_.ErrorAtPrevious("Expected expression.");
    return;
  }

  (this->*prefixRule)();

  while (precedence <= GetParseRule(parser_.get_current().type).precedence) {
    parser_.Advance();
    auto infix_rule = GetParseRule(parser_.get_previous().type).infix_func;
    (this->*infix_rule)();
  }
}

void Compiler::StopCompiling() {
  EmitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!parser_.had_error()) {
    GetCurrentChunk()->Disassemble("code");
  }
#endif
}

void Compiler::String() {
  GetCurrentChunk()->WriteConstant(
      std::string{parser_.get_previous().lexeme.begin() + 1,
                  parser_.get_previous().lexeme.end() - 1},
      parser_.get_previous().line);
}

void Compiler::Unary() {
  auto operator_type = parser_.get_previous().type;

  // Compile the operand
  ParsePrecedence(Precedence::kUnary);

  switch (operator_type) {
    case TokenType::kBang:
      EmitByte(Opcode::kNot);
      break;
    case TokenType::kMinus:
      EmitByte(Opcode::kNegate);
      break;
    default:
      return;
  }
}

}  // namespace lox
