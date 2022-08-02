// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_SCANNER_H
#define LOX_SRC_SCANNER_H

#include "token.h"

namespace lox {

class Scanner {
 public:
  explicit Scanner(std::string_view source);

  char Advance();
  Token ScanToken();

 private:
  [[nodiscard]] bool IsAtEnd() const;
  [[nodiscard]] char Peek() const;
  [[nodiscard]] char PeekNext() const;
  bool Match(char expected);
  [[nodiscard]] Token MakeToken(TokenType type) const;
  [[nodiscard]] Token MakeErrorToken(std::string_view message) const;
  void SkipWhitespace();
  TokenType CheckKeyword(std::size_t start, std::size_t length,
                         const char *rest, TokenType type) const;
  TokenType FindIdentifierType() const;
  Token HandleIdentifier();
  Token HandleNumber();
  Token HandleString();

  const char *start_ = nullptr;
  const char *current_ = nullptr;
  std::size_t line_{1};
};

}  // namespace lox

#endif  // LOX_SRC_SCANNER_H
