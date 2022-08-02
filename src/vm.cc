// SPDX-License-Identifier: Apache-2.0

#include "vm.h"

#include <array>
#include <iostream>

namespace lox {

template <typename Operator>
bool VirtualMachine::BinaryOp(const std::uint8_t *ip, Operator op) {
  if (!(std::holds_alternative<double>(Peek(0)) &&
        std::holds_alternative<double>(Peek(1)))) {
    RuntimeError(ip, "Operands must be numbers.");
    return false;
  }

  auto b = std::get<double>(PopValue());
  auto a = std::get<double>(PopValue());
  PushValue(op(a, b));
  return true;
}

void VirtualMachine::PushValue(Value value) { stack_.push_back(value); }

Value VirtualMachine::PopValue() {
  Value result = *(stack_.end() - 1);
  stack_.pop_back();
  return result;
}

Value &VirtualMachine::Peek(long distance) {
  return *(stack_.end() - distance - 1);
}

InterpretResult VirtualMachine::Run() {
#define BINARY_OP(op)                                                        \
  do {                                                                       \
    if (!BinaryOp(ip, [](double a, double b) -> Value { return a op b; })) { \
      return InterpretResult::kRuntimeError;                                 \
    }                                                                        \
  } while (false)

  const std::uint8_t *ip = chunk_->GetCodePtr();

  auto read_byte = [&ip]() -> std::uint8_t { return *ip++; };

  auto read_constant = [this, read_byte]() -> Value {
    return this->chunk_->GetValueAtIndex(read_byte());
  };

  auto last_element = [this]() -> const Value & {
    return *(this->stack_.end() - 1);
  };

  while (true) {
#if DEBUG_TRACE_EXECUTION
    std::cout << "          ";
    for (auto &slot : stack_) {
      std::cout << "[ " << slot << " ]";
    }
    std::cout << '\n';
    chunk_->DisassembleInstruction(
        static_cast<std::size_t>(ip - chunk_->GetCodePtr()));
#endif
    auto instruction = static_cast<Opcode>(read_byte());
    switch (instruction) {
      case Opcode::kConstant: {
        Value constant = read_constant();
        PushValue(constant);
        break;
      }
      case Opcode::kConstantLong: {
        auto constant_bytes = std::array<std::uint8_t, 3>{};
        for (auto i = std::size_t{0}; i < constant_bytes.size(); ++i) {
          constant_bytes[i] = read_byte();
        }
        auto constant = (constant_bytes[0] << 16) | (constant_bytes[1] << 8) |
                        constant_bytes[2];
        PushValue(static_cast<double>(constant));
        break;
      }
      case Opcode::kNil:
        PushValue(std::monostate{});
        break;
      case Opcode::kTrue:
        PushValue(true);
        break;
      case Opcode::kFalse:
        PushValue(false);
        break;
      case Opcode::kEqual: {
        auto b = PopValue();
        auto a = PopValue();
        PushValue(a == b);
        break;
      }
      case Opcode::kNotEqual: {
        auto b = PopValue();
        auto a = PopValue();
        PushValue(a != b);
        break;
      }
      case Opcode::kGreater:
        BINARY_OP(>);
        break;
      case Opcode::kGreaterEqual:
        BINARY_OP(>=);
        break;
      case Opcode::kLess:
        BINARY_OP(<);
        break;
      case Opcode::kLessEqual:
        BINARY_OP(<=);
        break;
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
      case Opcode::kNot:
        stack_[stack_.size() - 1] = IsFalsey(last_element());
        break;
      case Opcode::kNegate: {
        if (!std::holds_alternative<double>(Peek(0))) {
          RuntimeError(ip, "Operand must be a number");
          return InterpretResult::kRuntimeError;
        }
        stack_[stack_.size() - 1] = -std::get<double>(last_element());
        break;
      }
      case Opcode::kReturn:
        std::cout << PopValue() << '\n';
        return InterpretResult::kOk;
    }
  }
#undef BINARY_OP
}

template <typename Arg, typename... Args>
void VirtualMachine::RuntimeError(const std::uint8_t *ip, Arg &&arg,
                                  Args &&...args) {
  // From
  // https://stackoverflow.com/questions/27375089/what-is-the-easiest-way-to-print-a-variadic-parameter-pack-using-stdostream
  std::cerr << std::forward<Arg>(arg);
  (std::cerr << ... << std::forward<Args>(args));
  std::cerr << "\n";

  auto instruction = static_cast<std::size_t>(ip - chunk_->GetCodePtr() - 1);
  std::size_t line = chunk_->GetLineAtIndex(instruction);
  std::cerr << "[line " << line << "] in script\n";
  stack_.clear();
}

InterpretResult VirtualMachine::Interpret(std::string_view source) {
  auto compiler = Compiler{source};
  auto chunk = Chunk{};

  if (!compiler.Compile(&chunk)) return InterpretResult::kCompileError;

  chunk_ = &chunk;

  InterpretResult result = Run();
  chunk_ = nullptr;
  return result;
}

}  // namespace lox
