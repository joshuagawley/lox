// SPDX-License-Identifier: Apache-2.0

#ifndef LOX_SRC_VALUE_H
#define LOX_SRC_VALUE_H

#include <ostream>
#include <variant>

namespace lox {

namespace internal {

struct FalsinessVisitor {
  constexpr bool operator()(const bool b) { return !b; }

  constexpr bool operator()(const std::monostate /* unused */) { return true; }

  template <typename T>
  constexpr bool operator()(const T & /* unused */) {
    return false;
  }
};

struct OstreamVisitor {
  std::ostream &os;

  void operator()(const std::monostate /* unused */) const { os << "nil"; }

  void operator()(const double value) const { os << value; }

  void operator()(const bool value) const {
    os << std::boolalpha << value << std::noboolalpha;
  }
};

}  // namespace internal

using Value = std::variant<std::monostate, double, bool>;

inline std::ostream &operator<<(std::ostream &os, const Value &value) {
  std::visit(internal::OstreamVisitor{os}, value);
  return os;
}

constexpr bool IsFalsey(const Value &value) {
  return std::visit(internal::FalsinessVisitor{}, value);
}

}  // namespace lox

#endif  // LOX_SRC_VALUE_H
