#pragma once

#include "ast.h"
#include "ir.h"
#include "opt_config.h"
#include "semantic.h"

#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::to_string;
using std::vector;

class CodeGen {
public:
  CodeGen(Program &program, const Semantic &semantic, const O1Profile &o1 = O1Profile{});

  string run();

  /// Tier A+：Codegen 层 -O1 捷径（除常 magic、stride 等）
  bool optO1() const { return o1Profile_.codegen; }

  const O1Profile &o1Profile() const { return o1Profile_; }

  void emit(const string &line = "");

private:
  Program &program_;
  const Semantic &semantic_;
  O1Profile o1Profile_{};
  bool optO1_ = false;
  std::ostringstream out_;
  int labelId_ = 0;
  Function *currentFunction_ = nullptr;
  vector<string> breakLabels_;
  vector<string> continueLabels_;
  vector<pair<string, float>> floatLiterals_;
  vector<pair<string, string>> stringLiterals_;

  int irVregCount_ = 0;
  int irSpillBase_ = 0;
  int irTotalFrame_ = 0;
  vector<char> irVFloat_;
  /// IR 整型 vreg 是否为 XLEN 指针（与 irVFloat_ 互斥）
  vector<char> irVPtr_;
  vector<int> irVregSlots_;
  int irLastVregInA0_ = -1;
  int irLastVregInFa0_ = -1;
  // Multi-register cache: keep 3 int vregs in a0/t4/t5, 2 float in fa0/ft0
  int irCacheT4_ = -1;
  int irCacheT5_ = -1;
  int irCacheFt0_ = -1;
  bool irSkipStore_ = false;
  std::vector<bool> irSkipStoreVregs_;
  bool irSkippedLast_ = false;
  int irSkippedVreg_ = -1;
  std::unordered_map<Symbol *, std::string> irParamCache_;
  std::unordered_map<Symbol *, Expr *> inlineArgMap_;

  int irVregSlotOffset(int vid) const;

  bool irVregIsPtr(int vid) const;

  void emitIrLoadVreg(int vid, bool asFloat);

  void emitIrLoadVregTo(int vid, const string &reg);

  void emitIrStoreVreg(int vid, bool asFloat);

  void emitIrFlushSkipped();

  void emitIrFunction(FuncDef &def, IRFunction &ir);

  /// 在发射 IR 前推断各整型 vreg 是否为 XLEN 指针（与 ir_opt 后的 Copy/Add 一致）
  void irInferPtrRegs(const IRFunction &ir);

  void emitIrInst(FuncDef &def, const IRFunction &ir, const IRInst &in, size_t instIdx);

  void emitIrCall(FuncDef &def, const IRFunction &ir, const IRInst &in, size_t instIdx);

  string newLabel(const string &prefix);

  static bool fitsImm12(int value);

  void emitAdjustSp(int delta);

  void emitAddOffset(const string &dst, const string &base, int offset);

  void emitLoadMem(const string &inst, const string &dst, const string &base,
                   int offset);

  void emitStoreMem(const string &inst, const string &src, const string &base,
                    int offset);

  void emitGlobals();

  void emitFunction(FuncDef &def);

  void emitStoreParams(FuncDef &def);

  void emitBlock(BlockStmt &block, bool);

  void emitStmt(Stmt *stmt);

  void emitDecl(DeclStmt &decl);

  int flattenRuntimeInit(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<InitVal *> &flat);

  size_t chooseRuntimeInitChildDepth(const vector<int> &dims, size_t depth,
                                     int flatIndex);

  template <class AddressEmitter>
  void emitStoreToAddress(BaseType base, AddressEmitter emitAddressToA1);

  void emitAssign(AssignStmt &stmt);

  void emitIf(IfStmt &stmt);

  void emitWhile(WhileStmt &stmt);

  void emitReturn(ReturnStmt &stmt);

  void emitExpr(Expr *expr);

  void emitConst(ConstValue v);

  void emitFloatConst(float value);

  void emitStringExpr(StringExpr *expr);

  void emitUnary(UnaryExpr *expr);

  void emitBinary(BinaryExpr *expr);

  void emitIntCompare(const string &op);

  void emitFloatCompare(const string &op);

  void emitCond(Expr *expr, const string &trueLabel, const string &falseLabel);

  bool tryEmitCondBranchFalse(Expr *expr, const string &falseLabel);

  void emitBoolFromValue(const Type &type);

  void emitLValValue(LValExpr *expr);

  void emitLValAddress(LValExpr *expr);

  int strideForIndex(Symbol *sym, size_t index);

  void emitCall(CallExpr *expr);

  bool tryEmitInlineCall(CallExpr *expr);

  bool tryEmitTailCallReturn(ReturnStmt &stmt);

  void emitConvert(const Type &from, const Type &to);

  void emitPushInt(const string &reg);

  void emitPopInt(const string &reg);

  void emitPushFloat(const string &reg);

  void emitPopFloat(const string &reg);

  void emitLiteralPools();
};
