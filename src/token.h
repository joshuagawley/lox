// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_TOKEN_H
#define LOX_SRC_TOKEN_H

#include <cstddef>
#include <string_view>

namespace lox {

enum class TokenType {
  // clang-format off

  // Single character tokens.
  kLeftParen, kRightParen,
  kLeftBrace, kRightBrace,
  kComma, kDot, kMinus, kPlus,
  kSemicolon, kSlash, kStar,

  // One or two character tokens
  kBang, kBangEqual,
  kEqual, kEqualEqual,
  kGreater, kGreaterEqual,
  kLess, kLessEqual,

  // Literals
  kIdentifier, kString, kNumber,

  // Keywords
  kAnd, kClass, kElse, kFalse,
  kFor, kFun, kIf, kNil, kOr,
  kPrint, kReturn, kSuper, kThis,
  kTrue, kVar, kWhile,

  kError, kEof
  // clang-format on
};

struct Token {
  Token() = default;
  Token(TokenType type, const char *start, std::size_t length, std::size_t line)
      : type(type), lexeme(start, length), line(line){};
  Token(TokenType type, std::string_view lexeme, std::size_t line)
      : type(type), lexeme(lexeme), line(line){};

  TokenType type{TokenType::kEof};
  std::string_view lexeme{};
  std::size_t line{0};
};

}  // namespace lox

#endif  // LOX_SRC_TOKEN_H
