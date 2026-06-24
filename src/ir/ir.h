#pragma once

#include "common/ops.h"
#include <memory>
#include <string>
#include <vector>

namespace ir {

class IRBase {
public:
  virtual std::string to_string() const = 0;
  virtual ~IRBase() {};
};

class IRIdentifier : public IRBase {
public:
  IRIdentifier(std::string name);
  std::string to_string() const override { return _name; }

protected:
  std::string _name;
};

class Variable : public IRIdentifier {
public:
  Variable(std::string name);
};

class Label : public IRIdentifier {
public:
  Label(std::string name);
};

class FunctionName : public IRIdentifier {
public:
  FunctionName(std::string name);
};

class Number : public IRBase {
public:
  Number(long long val);
  std::string to_string() const override;

private:
  long long _val;
};

class Op : public IRBase {
public:
  Op(EOp op);
  std::string to_string() const override;

private:
  EOp _op;
};

namespace instr {

// class Label;
// class Assign;
// class AssignOp;
// class Call;
// class AssignCall;
// class ArrGet;
// class ArrSet;
// class Length;
// class Branch;
// class Return;

// using Instr = std::variant<Label, Assign, AssignOp, Call, AssignCall, ArrGet,
//                            ArrSet, Length, Branch, Return>;

class Instr : public IRBase {
public:
  Instr(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
protected:
  std::vector<std::unique_ptr<IRBase>> _ops;
  Instr *successor;
};

class Label : public Instr {
public:
  Label(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class Assign : public Instr {
public:
  Assign(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class AssignOp : public Instr {
public:
  AssignOp(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class Call : public Instr {
public:
  Call(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class AssignCall : public Instr {
public:
  AssignCall(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class ArrGet : public Instr {
public:
  ArrGet(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class ArrSet : public Instr {
public:
  ArrSet(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class Length : public Instr {
public:
  Length(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class Branch : public Instr {
public:
  Branch(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

class Return : public Instr {
public:
  Return(std::vector<std::unique_ptr<IRBase>> ops);
  std::string to_string() const override;
};

} // namespace instr

class BasicBlock : public IRBase {
public:
  std::string to_string() const override;

  void add_instr(std::unique_ptr<instr::Instr> instr);

private:
  std::vector<std::unique_ptr<instr::Instr>> _instrs;
};

class Function : public IRBase {
public:
  Function(std::unique_ptr<FunctionName> name);
  std::string to_string() const override;

  void add_block(std::unique_ptr<BasicBlock> block);
private:
  std::unique_ptr<FunctionName> _name;
  std::vector<std::unique_ptr<BasicBlock>> _blocks;
};

class Program : public IRBase {
public:
  std::string to_string() const override;

  void add_func(std::unique_ptr<Function> func);
private:
  std::vector<std::unique_ptr<Function>> _funcs;
};

} // namespace ir