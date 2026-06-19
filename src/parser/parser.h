#pragma once

#include "ast.h"

namespace parser {
  Program parse_file(const char *filename);
}
