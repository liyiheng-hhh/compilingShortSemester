#pragma once

// 中端 IR：用于 -O1 下可走 IR 的函数体（局部数组总元素数有上限；表达式无 && / ||）。
// 控制流用 Label / J / Beqz 插在 flat insts 中；irRefreshCFG 按 leader 划分基本块并填 succ，
// 供活跃变量与槽位分配；发射仍遍历 insts。

#include "ast.h"
#include "opt_config.h"

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
  Sra,
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
  Label,
  J,
  Beqz,
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

// 基本块：半开区间 [begin, end) 指向 IRFunction::insts；succ 为后继块下标
struct IRBlock {
  size_t begin = 0;
  size_t end = 0;
  std::vector<int> succ;
};

struct IRFunction {
  std::string name;
  BaseType ret = BaseType::Int;
  int nextVreg = 0;
  std::vector<IRInst> insts;
  std::vector<IRBlock> blocks;
  int entryBlockIndex = 0;
  // 按需计算的 vreg→槽号 映射（每个槽 8 字节），-1 表示未分配
  std::vector<int> vregSlots;

  int allocVreg() { return nextVreg++; }
};

bool irFunctionEligible(const FuncDef &def);

bool irExprHasLogicalShortCircuit(Expr *e);

void irBuildFunction(FuncDef &def, const Semantic &semantic, IRFunction &out);

// 由 insts 划分 leaders、填充 blocks[].begin/end/succ；insts 为空则 blocks 清空
void irRefreshCFG(IRFunction &fn);

void irOptimizeBlock(IRFunction &fn, const O1Profile &profile,
                     const Semantic *semantic = nullptr);

void irAssignSlots(IRFunction &fn);

int irSlotCount(const IRFunction &fn);
