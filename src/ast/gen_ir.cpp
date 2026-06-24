#include "ast.h"

namespace ast {
void gen_ir(std::unique_ptr<Program> &root) {
  Context ctx;
  root->gen_ir(ctx);
}

} // namespace ast