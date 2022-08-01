// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_PARSER_H
#define LOX_SRC_PARSER_H

#include <string_view>

#include "scanner.h"
#include "token.h"

namespace lox {

class Parser {
 public:
  explicit Parser(std::string_view source);

  void Advance();
  void Consume(TokenType type, std::string_view message);
  Token &get_current();
  Token &get_previous();
  [[nodiscard]] bool had_error() const;

  void ErrorAtCurrent(std::string_view message);
  void ErrorAtPrevious(std::string_view message);

 private:
  bool panic_mode_ = false;
  bool had_error_ = false;
  Token current_;
  Token previous_;
  Scanner scanner_;

  void ErrorAt(const Token &token, std::string_view message);
};

}  // namespace lox

#endif  // LOX_SRC_PARSER_H
