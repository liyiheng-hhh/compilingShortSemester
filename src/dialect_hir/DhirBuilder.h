#ifndef HIR_BUILDER_H
#define HIR_BUILDER_H

#include "DhirOps.h"

namespace sys::dhir {

class Builder {
public:
  Module build(ASTNode *node);

private:
  std::unique_ptr<Op> buildNode(ASTNode *node);
  std::unique_ptr<Op> buildBlockLike(ASTNode *node);
};

}  // namespace sys::dhir

#endif
