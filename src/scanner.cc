// SPDX-License-Identifier: Apache-2.0

#include "scanner.h"

#include <cstring>
#include <locale>

#define CHECK_FOR_CHAR_OR_END(c) (Peek() == c || IsAtEnd())

namespace lox {

auto Scanner::operator()(std::string_view source) noexcept -> void {
  start_ = &source[0];
  current_ = &source[0];
  line_ = 1;
}

auto Scanner::ScanToken() -> Token {
  SkipWhitespace();
  start_ = current_;

  if (IsAtEnd()) return MakeToken(TokenType::kEof);

  auto c = Advance();

  if (std::isalpha(c)) return HandleIdentifier();
  if (std::isdigit(c)) return HandleNumber();

  switch (c) {
    case '(':
      return MakeToken(TokenType::kLeftParen);
    case ')':
      return MakeToken(TokenType::kRightParen);
    case '{':
      return MakeToken(TokenType::kLeftBrace);
    case '}':
      return MakeToken(TokenType::kRightBrace);
    case ';':
      return MakeToken(TokenType::kSemicolon);
    case ',':
      return MakeToken(TokenType::kComma);
    case '.':
      return MakeToken(TokenType::kDot);
    case '-':
      return MakeToken(TokenType::kMinus);
    case '+':
      return MakeToken(TokenType::kPlus);
    case '/':
      return MakeToken(TokenType::kSlash);
    case '*':
      return MakeToken(TokenType::kStar);
    case '!':
      return MakeToken(Match('=') ? TokenType::kBangEqual : TokenType::kBang);
    case '=':
      return MakeToken(Match('=') ? TokenType::kEqualEqual : TokenType::kEqual);
    case '<':
      return MakeToken(Match('=') ? TokenType::kLessEqual : TokenType::kLess);
    case '>':
      return MakeToken(Match('=') ? TokenType::kGreaterEqual
                                  : TokenType::kGreater);
    case '"':
      return HandleString();
  }

  return MakeErrorToken("Unexpected character.");
}

auto Scanner::IsAtEnd() -> bool { return *current_ == '\0'; }

auto Scanner::Advance() -> char {
  current_++;
  return current_[-1];
}

auto Scanner::Peek() -> char { return *current_; }

auto Scanner::PeekNext() -> char {
  if (IsAtEnd()) return '\0';
  return current_[1];
}

auto Scanner::Match(const char expected) -> bool {
  if (IsAtEnd()) return false;
  if (*current_ != expected) return false;
  current_++;
  return true;
}

auto Scanner::MakeToken(TokenType type) -> Token {
  return Token{type, start_, static_cast<std::size_t>(current_ - start_),
               line_};
}

auto Scanner::MakeErrorToken(std::string_view message) const -> Token {
  return Token{TokenType::kError, message, line_};
}

auto Scanner::SkipWhitespace() -> void {
  while (true) {
    auto c = Peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        Advance();
        break;
      case '/':
        if (PeekNext() == '/') {
          while (!CHECK_FOR_CHAR_OR_END('\n')) Advance();
        }
      default:
        return;
    }
  }
}

auto Scanner::CheckKeyword(std::size_t start, std::size_t length,
                           const char *rest, TokenType type) -> TokenType {
  if (current_ - start_ == start + length &&
      std::memcmp(start_ + start, rest, length) == 0) {
    return type;
  }
  return TokenType::kIdentifier;
}

auto Scanner::FindIdentifierType() -> TokenType {
  switch (start_[0]) {
    case 'a':
      return CheckKeyword(1, 2, "nd", TokenType::kAnd);
    case 'c':
      return CheckKeyword(1, 4, "lass", TokenType::kClass);
    case 'e':
      return CheckKeyword(1, 3, "lse", TokenType::kElse);
    case 'f':
      if (current_ - start_ > 1) {
        switch (start_[1]) {
          case 'a':
            return CheckKeyword(2, 3, "lse", TokenType::kElse);
          case 'o':
            return CheckKeyword(2, 1, "r", TokenType::kFor);
          case 'u':
            return CheckKeyword(2, 1, "n", TokenType::kFun);
        }
      }
      break;
    case 'i':
      return CheckKeyword(1, 1, "f", TokenType::kIf);
    case 'n':
      return CheckKeyword(1, 2, "il", TokenType::kNil);
    case 'o':
      return CheckKeyword(1, 1, "r", TokenType::kOr);
    case 'p':
      return CheckKeyword(1, 4, "rint", TokenType::kPrint);
    case 'r':
      return CheckKeyword(1, 5, "eturn", TokenType::kReturn);
    case 's':
      return CheckKeyword(1, 4, "per", TokenType::kSuper);
    case 't':
      if (current_ - start_ > 1) {
        switch (start_[1]) {
          case 'h':
            return CheckKeyword(2, 2, "this", TokenType::kThis);
          case 'r':
            return CheckKeyword(2, 2, "ue", TokenType::kTrue);
        }
      }
      break;
    case 'v':
      return CheckKeyword(1, 2, "ar", TokenType::kVar);
    case 'w':
      return CheckKeyword(1, 4, "hile", TokenType::kWhile);
  }
}

auto Scanner::HandleIdentifier() -> Token {
  while (std::isalnum(Peek())) Advance();
  return MakeToken(FindIdentifierType());
}

auto Scanner::HandleNumber() -> Token {
  while (std::isdigit(Peek())) Advance();

  // Look for a fractional part
  if (Peek() == '.' && std::isdigit(PeekNext())) {
    // Consume the "."
    Advance();

    while (std::isdigit(Peek())) Advance();
  }

  return MakeToken(TokenType::kNumber);
}

auto Scanner::HandleString() -> Token {
  while (!CHECK_FOR_CHAR_OR_END('"')) {
    if (Peek() == '\n') line_++;
    Advance();
  }

  if (IsAtEnd()) return MakeErrorToken("Unterminated string.");

  Advance();
  return MakeToken(TokenType::kString);
}

}  // namespace lox