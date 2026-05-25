#define MATCH_TERNARY(opcode, Ty) \
  if (opname == opcode && isa<Ty>(op)) { \
    return mtMatchExpr(list->elements[1], op->getOperand(0).defining) && \
           mtMatchExpr(list->elements[2], op->getOperand(1).defining) && \
           mtMatchExpr(list->elements[3], op->getOperand(2).defining); \
  }

#define MATCH_BINARY(opcode, Ty) \
  if (opname == opcode && isa<Ty>(op)) { \
    return mtMatchExpr(list->elements[1], op->getOperand(0).defining) && \
           mtMatchExpr(list->elements[2], op->getOperand(1).defining); \
  }

#define MATCH_UNARY(opcode, Ty) \
  if (opname == opcode && isa<Ty>(op)) { \
    return mtMatchExpr(list->elements[1], op->getOperand(0).defining); \
  }

#define EVAL_BINARY(opcode, op) \
  if (opname == "!" opcode) { \
    int a = mtEvalExpr(list->elements[1]); \
    int b = mtEvalExpr(list->elements[2]); \
    return a op b; \
  }

#define EVAL_UNARY(opcode, op) \
  if (opname == "!" opcode) { \
    int a = mtEvalExpr(list->elements[1]); \
    return op a; \
  }

#define EVAL_BINARY_F_OPERAND(opcode, op) \
  if (opname == "!" opcode) { \
    float a = mtEvalFExpr(list->elements[1]); \
    float b = mtEvalFExpr(list->elements[2]); \
    return a op b; \
  }

#define EVAL_UNARY_F_OPERAND(opcode, op) \
  if (opname == "!" opcode) { \
    float a = mtEvalFExpr(list->elements[1]); \
    return op a; \
  }

#define EVAL_BINARY_F(opcode, op) \
  if (opname == "?" opcode) { \
    float a = mtEvalFExpr(list->elements[1]); \
    float b = mtEvalFExpr(list->elements[2]); \
    return a op b; \
  }

#define EVAL_UNARY_F(opcode, op) \
  if (opname == "?" opcode) { \
    float a = mtEvalFExpr(list->elements[1]); \
    return op a; \
  }

#define EVAL_UNARY_I_OPERAND(opcode, op) \
  if (opname == "?" opcode) { \
    int a = mtEvalExpr(list->elements[1]); \
    return op a; \
  }

#define BUILD_TERNARY(opcode, Ty) \
  if (opname == opcode) { \
    Value a = mtBuildExpr(list->elements[1]); \
    Value b = mtBuildExpr(list->elements[2]); \
    Value c = mtBuildExpr(list->elements[3]); \
    return builder.create<Ty>({ a, b, c }); \
  }

#define BUILD_BINARY(opcode, Ty) \
  if (opname == opcode) { \
    Value a = mtBuildExpr(list->elements[1]); \
    Value b = mtBuildExpr(list->elements[2]); \
    return builder.create<Ty>({ a, b }); \
  }

#define BUILD_UNARY(opcode, Ty) \
  if (opname == opcode) { \
    Value a = mtBuildExpr(list->elements[1]); \
    return builder.create<Ty>({ a }); \
  }
