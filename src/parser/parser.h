#pragma once

#include "ast/ast.h"

namespace ast {
  ast::Program parse_file(const char *filename);
}
