#pragma once

#include "context.h"
#include <string>
#include <vector>

namespace utils {
  class IdentifierGenerator;
}

namespace ast {

class ASTNode {
public:
  ASTNode() = default;
  ASTNode(ASTNode &&other) { _children = std::move(other._children); }
  ASTNode(std::vector<std::unique_ptr<ASTNode>> children);
  std::string children_to_string() const;
  virtual std::string to_string() const = 0;
  virtual ~ASTNode() {}
  virtual void gen_ir(Context &c) {}

  void add_child(std::unique_ptr<ASTNode> child);

protected:
  std::vector<std::unique_ptr<ASTNode>> _children;
};

class Identifier : public ASTNode {
public:
  Identifier(std::string name);
  std::string to_string() const override;

private:
  std::string _name;
};

class Number : public ASTNode {
public:
  Number(long long val);
  std::string to_string() const override;

private:
  long long _val;
};

enum EOp { ADD, SUB, MUL, AND, L_SHIFT, R_SHIFT, LT, LTEQ, EQ, GTEQ, GT };
class Op : public ASTNode {
public:
  Op(EOp op);
  std::string to_string() const override;

private:
  EOp _op;
};

enum EType { INT64, ARRAY, VOID };
class Type : public ASTNode {
public:
  Type(EType type);
  std::string to_string() const override;

private:
  EType _type;
};

class ArrayType : public Type {
public:
  ArrayType(uint64_t dims);
  std::string to_string() const override;

private:
  uint64_t _dims;
};

class Params : public ASTNode {
public:
  Params() = default;
  Params(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Args : public ASTNode {
public:
  Args() = default;
  Args(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Cond : public ASTNode {
public:
  Cond(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

namespace instr {

class Decl : public ASTNode {
public:
  Decl(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Assign : public ASTNode {
public:
  Assign(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class AssignOp : public ASTNode {
public:
  AssignOp(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Label : public ASTNode {
public:
  Label(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class If : public ASTNode {
public:
  If(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class While : public ASTNode {
public:
  While(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Continue : public ASTNode {
public:
  std::string to_string() const override;
};

class Break : public ASTNode {
public:
  std::string to_string() const override;
};

class ArrGet : public ASTNode {
public:
  ArrGet(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class ArrSet : public ASTNode {
public:
  ArrSet(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Length : public ASTNode {
public:
  Length(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Call : public ASTNode {
public:
  Call(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class AssignCall : public ASTNode {
public:
  AssignCall(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class CreateArray : public ASTNode {
public:
  CreateArray(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Return : public ASTNode {
public:
  Return() = default;
  Return(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Goto : public ASTNode {
public:
  Goto(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

} // namespace instr

class Scope : public ASTNode {
public:
  Scope() = default;
  Scope(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Function : public ASTNode {
public:
  Function() = default;
  Function(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

class Program : public ASTNode {
public:
  Program() = default;
  Program(std::vector<std::unique_ptr<ASTNode>> children);
  std::string to_string() const override;
};

void gen_ir();

} // namespace ast