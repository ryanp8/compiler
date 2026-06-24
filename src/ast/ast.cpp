
#include <iostream>
#include <memory>
#include <string>

#include "ast.h"
#include "utils/identifier_generator.h"

namespace ast {

ASTNode::ASTNode(std::vector<std::unique_ptr<ASTNode>> children)
    : _children(std::move(children)) {}

void ASTNode::add_child(std::unique_ptr<ASTNode> child) {
  _children.push_back(std::move(child));
}

std::string ASTNode::children_to_string() const {
  std::string result;
  for (auto it = _children.begin(); it != _children.end(); it++) {
    result += (*it)->to_string();
    if (it != _children.end() - 1) {
      result += ", ";
    }
  }
  return result;
}

Identifier::Identifier(std::string name) : _name(std::move(name)) {}
std::string Identifier::to_string() const { return _name; }

Number::Number(long long val) : _val(val) {}
std::string Number::to_string() const { return std::to_string(_val); }

Op::Op(EOp op) : _op(op) {}
std::string Op::to_string() const {
  switch (_op) {
  case EOp::ADD:
    return "+";
  case EOp::SUB:
    return "-";
  case EOp::MUL:
    return "*";
  case EOp::AND:
    return "&";
  case EOp::L_SHIFT:
    return "<<";
  case EOp::R_SHIFT:
    return ">>";
  case EOp::LT:
    return "<";
  case EOp::LTEQ:
    return "<=";
  case EOp::GT:
    return ">";
  case EOp::GTEQ:
    return ">=";
  case EOp::EQ:
    return "=";
  default:
    std::cerr << "Invalid EOp in to_string()\n";
    throw;
  }
}

Type::Type(EType type) : _type(type) {}
std::string Type::to_string() const {
  switch (_type) {
  case VOID:
    return "void";
  case INT64:
    return "int64";
  default:
    std::cerr << "Type to_string received incorrect EType " << _type << "\n";
    throw;
  }
}

ArrayType::ArrayType(uint64_t dims) : Type(ARRAY), _dims(dims) {}
std::string ArrayType::to_string() const {
  std::string s = "int64";
  for (size_t i = 0; i < _dims; i++) {
    s += "[]";
  }
  return s;
}

Params::Params(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Params::to_string() const {
  return "{ Params: " + ASTNode::children_to_string() + " }";
}

Args::Args(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Args::to_string() const {
  return "{ Args: " + ASTNode::children_to_string() + " }";
}

Cond::Cond(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Cond::to_string() const {
  return "{ Cond: " + ASTNode::children_to_string() + " }";
}

Scope::Scope(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Scope::to_string() const {
  return "{ Scope: " + ASTNode::children_to_string() + " }";
}

namespace instr {
Decl::Decl(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Decl::to_string() const {
  return "{ Decl: " + ASTNode::children_to_string() + " }";
}

Assign::Assign(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Assign::to_string() const {
  return "{ Assign: " + ASTNode::children_to_string() + " }";
}

AssignOp::AssignOp(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string AssignOp::to_string() const {
  return "{ AssignOp: " + ASTNode::children_to_string() + " }";
}

Label::Label(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Label::to_string() const {
  return "{ Label: " + ASTNode::children_to_string() + " }";
}

If::If(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string If::to_string() const {
  return "{ If: " + ASTNode::children_to_string() + " }";
}

While::While(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string While::to_string() const {
  return "{ While: " + ASTNode::children_to_string() + " }";
}

std::string Continue::to_string() const { return "{ Continue }"; }

std::string Break::to_string() const { return "{ Break }"; }

ArrGet::ArrGet(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string ArrGet::to_string() const {
  return "{ ArrGet: " + ASTNode::children_to_string() + " }";
}

ArrSet::ArrSet(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string ArrSet::to_string() const {
  return "{ ArrSet: " + ASTNode::children_to_string() + " }";
}

Length::Length(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Length::to_string() const {
  return "{ Length: " + ASTNode::children_to_string() + " }";
}

Call::Call(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Call::to_string() const {
  return "{ Call: " + ASTNode::children_to_string() + " }";
}

AssignCall::AssignCall(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string AssignCall::to_string() const {
  return "{ AssignCall: " + ASTNode::children_to_string() + " }";
}

CreateArray::CreateArray(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string CreateArray::to_string() const {
  return "{ CreateArray: " + ASTNode::children_to_string() + " }";
}

Return::Return(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Return::to_string() const {
  return "{ Return: " + ASTNode::children_to_string() + " }";
}

Goto::Goto(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Goto::to_string() const {
  return "{ Goto: " + ASTNode::children_to_string() + " }";
}
} // namespace instr

Function::Function(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Function::to_string() const {
  return "{ Function: " + ASTNode::children_to_string() + " }";
}

Program::Program(std::vector<std::unique_ptr<ASTNode>> children)
    : ASTNode(std::move(children)) {}
std::string Program::to_string() const {
  return "{ Program: " + ASTNode::children_to_string() + " }";
}

} // namespace ast