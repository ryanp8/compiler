#include "symbol_table.h"
#include "ast/ast.h"

namespace ast {
class Identifier;
class Type;
Type* SymbolTable::find(Identifier *i) {
  for (auto it = _st.rbegin(); it != _st.rend(); it++) {
    if (it->contains(i)) {
      return it->at(i);
    }
  }
  return nullptr;
}

void SymbolTable::insert(Identifier *i, Type *t) {
  if (_st.empty()) {
    _st.push_back({});
  }
  _st.back()[i] = t;
}
}