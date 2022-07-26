// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_VM_H
#define LOX_SRC_VM_H

#define DEBUG_TRACE_EXECUTION true

#include <vector>

#include "chunk.h"

namespace lox {

enum class InterpretResult { kOk, kCompileError, kRuntimeError };

class VirtualMachine {
 public:
  VirtualMachine() = default;
  ~VirtualMachine() = default;

  auto Interpret(Chunk *chunk) -> InterpretResult;
  auto PushValue(Value value);
  auto PopValue() -> Value;

 private:
  auto Run() -> InterpretResult;

  Chunk *chunk_ = nullptr;
  std::uint8_t *ip_ = nullptr;
  std::vector<Value> temps_;
};

}  // namespace lox

#endif  // LOX_SRC_VM_H
