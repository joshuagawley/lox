// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_SCANNER_H
#define LOX_SRC_SCANNER_H

#include <string_view>

#include "token.h"

namespace lox {

class Scanner {
 public:
  auto operator()(std::string_view source) noexcept -> void;

  auto ScanToken() -> Token;

 private:
  auto IsAtEnd() -> bool;
  auto Advance() -> char;
  auto Peek() -> char;
  auto PeekNext() -> char;
  auto Match(char expected) -> bool;
  auto MakeToken(TokenType type) -> Token;
  [[nodiscard]] auto MakeErrorToken(std::string_view message) const -> Token;
  auto SkipWhitespace() -> void;
  auto CheckKeyword(std::size_t start, std::size_t length, const char *rest,
                    TokenType type) -> TokenType;
  auto FindIdentifierType() -> TokenType;
  auto HandleIdentifier() -> Token;
  auto HandleNumber() -> Token;
  auto HandleString() -> Token;

  const char *start_ = nullptr;
  const char *current_ = nullptr;
  std::size_t line_{1};
};

}  // namespace lox

#endif  // LOX_SRC_SCANNER_H
