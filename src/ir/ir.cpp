#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "common/ops.h"
#include "ir.h"

namespace ir {

IRIdentifier::IRIdentifier(std::string name) : _name(std::move(name)) {}

Variable::Variable(std::string name) : IRIdentifier(std::move(name)) {}

Label::Label(std::string name) : IRIdentifier(std::move(name)) {}

FunctionName::FunctionName(std::string name) : IRIdentifier(std::move(name)) {}

Number::Number(long long val) : _val(val) {}
std::string Number::to_string() const { return std::to_string(_val); }

Op::Op(EOp op) : _op(op) {}
std::string Op::to_string() const { return op_to_string(_op); }

namespace instr {
Instr::Instr(std::vector<std::unique_ptr<IRBase>> ops)
    : _ops(std::move(ops)) {}
std::string Instr::to_string() const {
  std::string result;
  for (const auto &op_ptr : _ops) {
    result += op_ptr->to_string() + " ";
  }
  return result;
}

Label::Label(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string Label::to_string() const {
  return Instr::to_string();
}

Assign::Assign(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string Assign::to_string() const {
  return "assign " + Instr::to_string();
}

AssignOp::AssignOp(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string AssignOp::to_string() const {
  return "assignop " + Instr::to_string();
}

Call::Call(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string Call::to_string() const {
  return "call " + Instr::to_string();
}

ArrGet::ArrGet(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string ArrGet::to_string() const {
  return "arrget " + Instr::to_string();
}

ArrSet::ArrSet(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string ArrSet::to_string() const {
  return "arrset " + Instr::to_string();
}

Length::Length(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string Length::to_string() const {
  return "length " + Instr::to_string();
}

Branch::Branch(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string Branch::to_string() const {
  return "branch " + Instr::to_string();
}

Return::Return(std::vector<std::unique_ptr<IRBase>> ops) : Instr(std::move(ops)) {}
std::string Return::to_string() const {
  return "return " + Instr::to_string();
}

} // namespace instr

std::string Function::to_string() const {
  std::string result = _name->to_string() + "{\n";
  for (const auto& block : _blocks) {
    result += block->to_string() + "\n";
  }
  return result + "}\n";
}

} // namespace ir