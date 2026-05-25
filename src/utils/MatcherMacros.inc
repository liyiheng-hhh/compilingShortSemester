#define MATCH_TERNARY(opcode, Ty) \
  if (opname == opcode && isa<Ty>(op)) { \
    return matchExpr(list->elements[1], op->getOperand(0).defining) && \
           matchExpr(list->elements[2], op->getOperand(1).defining) && \
           matchExpr(list->elements[3], op->getOperand(2).defining); \
  }

#define MATCH_BINARY(opcode, Ty) \
  if (opname == opcode && isa<Ty>(op)) { \
    return matchExpr(list->elements[1], op->getOperand(0).defining) && \
           matchExpr(list->elements[2], op->getOperand(1).defining); \
  }

#define MATCH_UNARY(opcode, Ty) \
  if (opname == opcode && isa<Ty>(op)) { \
    return matchExpr(list->elements[1], op->getOperand(0).defining); \
  }

#define EVAL_BINARY(opcode, op) \
  if (opname == "!" opcode) { \
    int a = evalExpr(list->elements[1]); \
    int b = evalExpr(list->elements[2]); \
    return a op b; \
  }

#define EVAL_UNARY(opcode, op) \
  if (opname == "!" opcode) { \
    int a = evalExpr(list->elements[1]); \
    return op a; \
  }

#define EVAL_BINARY_F_OPERAND(opcode, op) \
  if (opname == "!" opcode) { \
    float a = evalFExpr(list->elements[1]); \
    float b = evalFExpr(list->elements[2]); \
    return a op b; \
  }

#define EVAL_UNARY_F_OPERAND(opcode, op) \
  if (opname == "!" opcode) { \
    float a = evalFExpr(list->elements[1]); \
    return op a; \
  }

#define EVAL_BINARY_F(opcode, op) \
  if (opname == "?" opcode) { \
    float a = evalFExpr(list->elements[1]); \
    float b = evalFExpr(list->elements[2]); \
    return a op b; \
  }

#define EVAL_UNARY_F(opcode, op) \
  if (opname == "?" opcode) { \
    float a = evalFExpr(list->elements[1]); \
    return op a; \
  }

#define EVAL_UNARY_I_OPERAND(opcode, op) \
  if (opname == "?" opcode) { \
    int a = evalExpr(list->elements[1]); \
    return op a; \
  }

#define BUILD_TERNARY(opcode, Ty) \
  if (opname == opcode) { \
    Value a = buildExpr(list->elements[1]); \
    Value b = buildExpr(list->elements[2]); \
    Value c = buildExpr(list->elements[3]); \
    return builder.create<Ty>({ a, b, c }); \
  }

#define BUILD_BINARY(opcode, Ty) \
  if (opname == opcode) { \
    Value a = buildExpr(list->elements[1]); \
    Value b = buildExpr(list->elements[2]); \
    return builder.create<Ty>({ a, b }); \
  }

#define BUILD_UNARY(opcode, Ty) \
  if (opname == opcode) { \
    Value a = buildExpr(list->elements[1]); \
    return builder.create<Ty>({ a }); \
  }
