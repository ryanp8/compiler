#pragma once

#include <unordered_map>
#include <vector>

namespace ast {
class Type;
class Identifier;
class SymbolTable {
public:
  SymbolTable() = default;
  Type *find(Identifier *i);
  void insert(Identifier *i, Type *t);

private:
  std::vector<std::unordered_map<Identifier *, Type *>> _st;
};
} // namespace ast
