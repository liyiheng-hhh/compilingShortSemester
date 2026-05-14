#pragma once

// 中端 IR：单基本块，用于 -O1 下「纯顺序」函数体（无 if/while/break/continue，
// 无局部数组声明，表达式无 && / ||）。irOptimizeBlock 做块内 CSE 与 COPY 折叠。

#include "ast.h"

#include <cstdint>
#include <string>
#include <vector>

class Semantic;

enum class IROp {
  Nop,
  ConstI,
  ConstF,
  Copy,
  LeaStr,
  LeaGlobal,
  LeaLocal,
  LoadParamAddr,
  LoadGlobal,
  StoreGlobal,
  LoadMem,
  StoreMem,
  LoadLocal,
  StoreLocal,
  Add,
  Sub,
  Mul,
  Sll,
  Div,
  Rem,
  Neg,
  FAdd,
  FSub,
  FMul,
  FDiv,
  FNeg,
  ICvtF,
  FCvtI,
  Slt,
  Seqz,
  Snez,
  FCmp,
  Call,
  Ret,
};

// FCmp 比较种类，放在 immI：0 == 1 != 2 < 3 > 4 <= 5 >=
enum : int32_t {
  FCMP_EQ = 0,
  FCMP_NE = 1,
  FCMP_LT = 2,
  FCMP_GT = 3,
  FCMP_LE = 4,
  FCMP_GE = 5,
};

struct IRInst {
  IROp op = IROp::Nop;
  int dst = -1;
  int u = -1;
  int v = -1;
  int32_t immI = 0;
  float immF = 0.0f;
  Symbol *sym = nullptr;
  std::string ext;
  std::string callee;
  std::vector<int> args;
  std::vector<char> callArgPtr;
  bool isFloat = false;
};

struct IRFunction {
  std::string name;
  BaseType ret = BaseType::Int;
  int nextVreg = 0;
  std::vector<IRInst> insts;
  // 按需计算的 vreg→槽号 映射（每个槽 8 字节），-1 表示未分配
  std::vector<int> vregSlots;

  int allocVreg() { return nextVreg++; }
};

bool irFunctionEligible(const FuncDef &def);

bool irExprHasLogicalShortCircuit(Expr *e);

void irBuildFunction(FuncDef &def, const Semantic &semantic, IRFunction &out);

void irOptimizeBlock(IRFunction &fn);

void irAssignSlots(IRFunction &fn);

int irSlotCount(const IRFunction &fn);
