// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_VM_H
#define LOX_SRC_VM_H

#define DEBUG_TRACE_EXECUTION true

#include <vector>

#include "chunk.h"
#include "compiler.h"

namespace lox {

enum class InterpretResult { kOk, kCompileError, kRuntimeError };

class VirtualMachine {
 public:
  VirtualMachine() = default;

  InterpretResult Interpret(std::string_view source);
  void PushValue(const Value &value);
  Value PopValue();

 private:
  template <typename Operator>
  bool BinaryOp(const std::uint8_t *ip, Operator op);

  Value &Peek(long distance);
  InterpretResult Run();
  /*
  template <typename Arg, typename... Args>
  void RuntimeError(const std::uint8_t *ip, Arg &&arg, Args &&...args);
  */
  void RuntimeError(const std::uint8_t *ip, const char *format...);

  Chunk *chunk_ = nullptr;
  std::vector<Value> stack_;
};

}  // namespace lox

#endif  // LOX_SRC_VM_H
