// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_SCANNER_H
#define LOX_SRC_SCANNER_H

#include <string_view>

#include "token.h"

namespace lox {

class Scanner {
 public:
  void operator()(std::string_view source) noexcept;

  char Advance();
  Token ScanToken();

 private:
  bool IsAtEnd();
  char Peek();
  char PeekNext();
  bool Match(char expected);
  Token MakeToken(TokenType type);
  [[nodiscard]] Token MakeErrorToken(std::string_view message) const;
  void SkipWhitespace();
  TokenType CheckKeyword(std::size_t start, std::size_t length,
                         const char *rest, TokenType type);
  TokenType FindIdentifierType();
  Token HandleIdentifier();
  Token HandleNumber();
  Token HandleString();

  const char *start_ = nullptr;
  const char *current_ = nullptr;
  std::size_t line_{1};
};

}  // namespace lox

#endif  // LOX_SRC_SCANNER_H
