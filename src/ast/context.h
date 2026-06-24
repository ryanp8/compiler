#pragma once

#include "ast/symbol_table.h"
#include "utils/identifier_generator.h"

namespace ast {
struct Context {
  SymbolTable symbols;
  utils::IdentifierGenerator label_gen;
  utils::IdentifierGenerator name_gen;
};
} // namespace ast