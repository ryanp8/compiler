#pragma once

#include <string>
#include <variant>
#include <vector>

namespace ir {

class Variable {
public:
  Variable(std::string name);
private:
  std::string _name;
};

class Label {
public:
  Label(std::string name);
private:
  std::string _name;
};

class Number {
public:
  Number(long long val);
private:
  long long _val;
};

class FunctionName {
public:
  FunctionName(std::string name);
private:
  std::string _name;
};

using Operand = std::variant<Variable, Label, Number, FunctionName>;

namespace instr {

class Label;
class Assign;
class AssignOp;
class Call;
class AssignCall;
class ArrGet;
class ArrSet;
class Length;
class Branch;
class Return;

using Instr = std::variant<Label, Assign, AssignOp, Call, AssignCall, ArrGet, ArrSet, Length, Branch, Return>;

class InstrBase {
protected:
  std::vector<std::unique_ptr<Operand>> _ops;
  Instr *successor;
};

class Label : public InstrBase {};

class Assign : public InstrBase {};

class AssignOp : public InstrBase {};

class Call : public InstrBase {};

class AssignCall : public InstrBase {};

class ArrGet : public InstrBase {};

class ArrSet : public InstrBase {};

class Length : public InstrBase {};

class Branch : public InstrBase {};

class Return : public InstrBase {};

} // namespace instr

class BasicBlock {
public:
  void add_instr(std::unique_ptr<instr::Instr> instr);

private:
  std::vector<std::unique_ptr<instr::Instr>> _instrs;
};

class Function {
public:
private:
  std::vector<std::unique_ptr<BasicBlock>> _blocks;
};

class Program {
public:
private:
  std::vector<std::unique_ptr<Function>> _funcs;
};

} // namespace ir