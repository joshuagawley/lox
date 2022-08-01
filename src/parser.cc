// SPDX-License-Identifier: Apache-2.0

#include "parser.h"

#include <iostream>

namespace lox {

Parser::Parser(std::string_view source)
    : current_(TokenType::kEof, source, 0),
      previous_(TokenType::kEof, source, 0),
      scanner_(source) {
  Advance();
}

void Parser::Advance() {
  previous_ = current_;

  while (true) {
    current_ = scanner_.ScanToken();
    if (current_.type != TokenType::kError) break;

    ErrorAtCurrent(current_.lexeme);
  }
}

void Parser::Consume(TokenType type, std::string_view message) {
  if (current_.type == type) {
    Advance();
  } else {
    ErrorAtCurrent(message);
  }
}

Token &Parser::get_current() { return current_; }

Token &Parser::get_previous() { return previous_; }

bool Parser::had_error() const { return had_error_; }

void Parser::ErrorAtCurrent(std::string_view message) {
  ErrorAt(current_, message);
}
void Parser::ErrorAtPrevious(std::string_view message) {
  ErrorAt(previous_, message);
}

void Parser::ErrorAt(const Token &token, std::string_view message) {
  if (panic_mode_) return;
  panic_mode_ = true;
  std::cerr << "[line " << token.line << " Error";

  if (token.type == TokenType::kEof) {
    std::cerr << " at end";
  } else if (token.type == TokenType::kError) {
  } else {
    std::cerr << " at '" << token.lexeme << "'";
  }

  std::cerr << ": " << message << '\n';
  had_error_ = true;
}

}  // namespace lox