// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_COMPILER_H
#define LOX_SRC_COMPILER_H

#include <string_view>

#include "scanner.h"

namespace lox {

class Compiler {
 public:
  auto Compile(std::string_view source) -> void;

 private:
  Scanner scanner_;
};

}  // namespace lox

#endif  // LOX_SRC_COMPILER_H
