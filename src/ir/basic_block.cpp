#include "ir.h"
#include <string>

namespace ir {
std::string BasicBlock::to_string() const {
  std::string result;
  for (const auto &instr : _instrs) {
    result += instr->to_string();
  }
  return result;
}
}