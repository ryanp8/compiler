#include <iostream>
#include <memory>

#include "ast/ast.h"
#include "parser/parser.h"


void print_help(char *progName) {
  std::cerr << "Usage: " << progName << "[-v] [-g 0|1] [-O 0|1|2] SOURCE"
            << std::endl;
  return;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_help(argv[0]);
    return 1;
  }

  ast::Program p = ast::parse_file(argv[1]);
  std::cout << p.to_string() << "\n";

  return 0;
}