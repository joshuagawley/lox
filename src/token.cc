// SPDX-License-Identifier: Apache-2.0

#include "token.h"

namespace lox {

Token::Token(TokenType type, const char *start, std::size_t length,
             std::size_t line)
    : type(type), lexeme(start, length), line(line) {}

Token::Token(TokenType type, std::string_view lexeme, std::size_t line)
    : type(type), lexeme(lexeme), line(line) {}

}  // namespace lox