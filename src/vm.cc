// SPDX-License-Identifier: Apache-2.0

#include "vm.h"

#include <array>
#include <iostream>

namespace lox {

auto VirtualMachine::Interpret(Chunk *chunk) -> InterpretResult {
  chunk_ = chunk;
  ip_ = chunk_->GetCodePtr();
  return Run();
}

auto VirtualMachine::PushValue(Value value) { temps_.push_back(value); }

auto VirtualMachine::PopValue() -> Value {
  auto result = *temps_.end();
  temps_.pop_back();
  return result;
}

auto VirtualMachine::Run() -> InterpretResult {
#define READ_BYTE() (*ip_++)
#define READ_CONSTANT() (chunk_->GetValueAtIndex(READ_BYTE()))
#define BINARY_OP(op)      \
  do {                     \
    double b = PopValue(); \
    double a = PopValue(); \
    PushValue(a op b);     \
  } while (false)

  while (true) {
#if DEBUG_TRACE_EXECUTION
    std::cout << "          ";
    for (auto slot : temps_) {
      std::cout << "[ " << slot << " ]";
    }
    std::cout << '\n';
    chunk_->DisassembleInstruction(ip_ - chunk_->GetCodePtr());
#endif
    auto instruction = static_cast<Opcode>(*ip_++);
    switch (instruction) {
      case Opcode::kConstant: {
        auto constant = READ_CONSTANT();
        PushValue(constant);
        break;
      }
      case Opcode::kConstantLong: {
        auto constant_bytes = std::array<std::uint8_t, 3>{};
        for (auto i = std::size_t{0}; i < constant_bytes.size(); ++i) {
          constant_bytes[i] = READ_BYTE();
        }
        auto constant = (constant_bytes[0] << 16) | (constant_bytes[1] << 8) |
                        constant_bytes[2];
        PushValue(constant);
        break;
      }
      case Opcode::kAdd:
        BINARY_OP(+);
        break;
      case Opcode::kSubtract:
        BINARY_OP(-);
        break;
      case Opcode::kMultiply:
        BINARY_OP(*);
        break;
      case Opcode::kDivide:
        BINARY_OP(/);
        break;
      case Opcode::kNegate: {
        temps_[temps_.size() - 1] = -temps_[temps_.size() - 1];
        break;
      }
      case Opcode::kReturn:
        std::cout << PopValue() << '\n';
        return InterpretResult::kOk;
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
}

}  // namespace lox