#include "codegen.h"

#include "common.h"

#include <algorithm>

using namespace std;

struct ArgLoc {
  enum Kind { IntReg, FloatReg, Stack } kind = IntReg;
  int index = 0;
  bool valueIsFloat = false;
};

static vector<ArgLoc> computeArgLocations(const vector<ParamType> &params,
                                          int *stackSlots = nullptr) {
  vector<ArgLoc> locs;
  int intRegs = 0;
  int floatRegs = 0;
  int stack = 0;
  for (const ParamType &p : params) {
    ArgLoc loc;
    loc.valueIsFloat = !p.isArray && p.base == BaseType::Float;
    if (loc.valueIsFloat && floatRegs < 8) {
      loc.kind = ArgLoc::FloatReg;
      loc.index = floatRegs++;
    } else {
      if (intRegs < 8) {
        loc.kind = ArgLoc::IntReg;
        loc.index = intRegs++;
      } else {
        loc.kind = ArgLoc::Stack;
        loc.index = stack++;
      }
    }
    locs.push_back(loc);
  }
  if (stackSlots) {
    *stackSlots = stack;
  }
  return locs;
}

CodeGen::CodeGen(Program &program, const Semantic &semantic)
    : program_(program), semantic_(semantic) {}

string CodeGen::run() {
    emitGlobals();
    emit("\t.text");
    for (auto &item : program_.items) {
      if (item.func) {
        emitFunction(*item.func);
      }
    }
    emitLiteralPools();
    return out_.str();
  }

void CodeGen::emit(const string &line) { out_ << line << '\n'; }

string CodeGen::newLabel(const string &prefix) {
    return ".L" + prefix + "_" + to_string(labelId_++);
  }

bool CodeGen::fitsImm12(int value) { return value >= -2048 && value <= 2047; }

void CodeGen::emitAdjustSp(int delta) {
    if (delta == 0) {
      return;
    }
    if (fitsImm12(delta)) {
      emit("\taddi\tsp, sp, " + to_string(delta));
    } else {
      emit("\tli\tt6, " + to_string(delta));
      emit("\tadd\tsp, sp, t6");
    }
  }

void CodeGen::emitAddOffset(const string &dst, const string &base, int offset) {
    if (fitsImm12(offset)) {
      emit("\taddi\t" + dst + ", " + base + ", " + to_string(offset));
    } else {
      emit("\tli\t" + dst + ", " + to_string(offset));
      emit("\tadd\t" + dst + ", " + base + ", " + dst);
    }
  }

void CodeGen::emitLoadMem(const string &inst, const string &dst, const string &base,
                   int offset) {
    if (fitsImm12(offset)) {
      emit("\t" + inst + "\t" + dst + ", " + to_string(offset) + "(" + base + ")");
    } else {
      emit("\tli\tt6, " + to_string(offset));
      emit("\tadd\tt6, " + base + ", t6");
      emit("\t" + inst + "\t" + dst + ", 0(t6)");
    }
  }

void CodeGen::emitStoreMem(const string &inst, const string &src, const string &base,
                    int offset) {
    if (fitsImm12(offset)) {
      emit("\t" + inst + "\t" + src + ", " + to_string(offset) + "(" + base + ")");
    } else {
      emit("\tli\tt6, " + to_string(offset));
      emit("\tadd\tt6, " + base + ", t6");
      emit("\t" + inst + "\t" + src + ", 0(t6)");
    }
  }

void CodeGen::emitGlobals() {
    for (Symbol *sym : semantic_.globalsInOrder()) {
      bool allZero = true;
      for (const ConstValue &v : sym->initValues) {
        if ((v.type == BaseType::Float && floatBits(v.f) != 0) ||
            (v.type == BaseType::Int && v.i != 0)) {
          allZero = false;
          break;
        }
      }
      if (sym->isConst) {
        emit("\t.section\t.rodata");
      } else if (allZero) {
        emit("\t.bss");
      } else {
        emit("\t.data");
      }
      emit("\t.align\t2");
      emit("\t.globl\t" + sym->label);
      emit(sym->label + ":");
      int count = sym->isArray ? product(sym->dims) : 1;
      if (allZero) {
        emit("\t.zero\t" + to_string(count * 4));
      } else {
        for (int i = 0; i < count; ++i) {
          ConstValue v = i < static_cast<int>(sym->initValues.size())
                             ? sym->initValues[i]
                             : ConstValue{sym->base, 0, 0.0f};
          if (sym->base == BaseType::Float) {
            emit("\t.word\t" + to_string(floatBits(constAsFloat(v))));
          } else {
            emit("\t.word\t" + to_string(static_cast<int32_t>(constAsInt(v))));
          }
        }
      }
    }
  }

void CodeGen::emitFunction(FuncDef &def) {
    currentFunction_ = def.function;
    currentFunction_->returnLabel = newLabel("return_" + def.name);
    emit("\t.text");
    emit("\t.align\t1");
    emit("\t.globl\t" + def.name);
    emit("\t.type\t" + def.name + ", @function");
    emit(def.name + ":");
    int frame = currentFunction_->frameSize;
    emitAdjustSp(-frame);
    emit("\tli\tt0, " + to_string(frame));
    emit("\tadd\tt0, sp, t0");
    emit("\tsd\tra, -8(t0)");
    emit("\tsd\ts0, -16(t0)");
    emit("\tmv\ts0, t0");
    emitStoreParams(def);
    emitBlock(*def.body, false);
    if (def.ret == BaseType::Int) {
      emit("\tli\ta0, 0");
    } else if (def.ret == BaseType::Float) {
      emit("\tfmv.w.x\tfa0, zero");
    }
    emit(currentFunction_->returnLabel + ":");
    emit("\tld\tra, -8(s0)");
    emit("\tld\tt0, -16(s0)");
    emit("\tmv\tsp, s0");
    emit("\tmv\ts0, t0");
    emit("\tret");
    emit("\t.size\t" + def.name + ", .-" + def.name);
    currentFunction_ = nullptr;
  }

void CodeGen::emitStoreParams(FuncDef &def) {
    vector<ParamType> types;
    for (const Param &p : def.params) {
      types.push_back(ParamType{p.base, p.isArray, p.dims});
    }
    auto locs = computeArgLocations(types);
    for (size_t i = 0; i < def.params.size(); ++i) {
      const Param &p = def.params[i];
      const ArgLoc &loc = locs[i];
      int off = p.symbol->offset;
      if (p.isArray) {
        if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sd", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("ld", "t0", "s0", loc.index * 8);
          emitStoreMem("sd", "t0", "s0", off);
        }
      } else if (p.base == BaseType::Float) {
        if (loc.kind == ArgLoc::FloatReg) {
          emitStoreMem("fsw", "fa" + to_string(loc.index), "s0", off);
        } else if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sw", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("lw", "t0", "s0", loc.index * 8);
          emitStoreMem("sw", "t0", "s0", off);
        }
      } else {
        if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sw", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("lw", "t0", "s0", loc.index * 8);
          emitStoreMem("sw", "t0", "s0", off);
        }
      }
    }
  }

void CodeGen::emitBlock(BlockStmt &block, bool) {
    for (auto &stmt : block.items) {
      emitStmt(stmt.get());
    }
  }

void CodeGen::emitStmt(Stmt *stmt) {
    switch (stmt->kind) {
    case StmtKind::Decl:
      emitDecl(*static_cast<DeclStmt *>(stmt));
      break;
    case StmtKind::Block:
      emitBlock(*static_cast<BlockStmt *>(stmt), true);
      break;
    case StmtKind::Assign:
      emitAssign(*static_cast<AssignStmt *>(stmt));
      break;
    case StmtKind::Expr: {
      ExprStmt *s = static_cast<ExprStmt *>(stmt);
      if (s->expr) {
        emitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::If:
      emitIf(*static_cast<IfStmt *>(stmt));
      break;
    case StmtKind::While:
      emitWhile(*static_cast<WhileStmt *>(stmt));
      break;
    case StmtKind::Break:
      if (breakLabels_.empty()) {
        throw CompileError("break outside loop");
      }
      emit("\tj\t" + breakLabels_.back());
      break;
    case StmtKind::Continue:
      if (continueLabels_.empty()) {
        throw CompileError("continue outside loop");
      }
      emit("\tj\t" + continueLabels_.back());
      break;
    case StmtKind::Return:
      emitReturn(*static_cast<ReturnStmt *>(stmt));
      break;
    }
  }

void CodeGen::emitDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      Symbol *sym = def.symbol;
      if (!sym->needsStorage) {
        continue;
      }
      if (!sym->isArray) {
        if (def.init && !def.init->isList) {
          emitExpr(def.init->expr.get());
          emitConvert(def.init->expr->type, Type::scalar(sym->base));
          emitStoreToAddress(sym->base, [this, sym]() {
            emitAddOffset("a1", "s0", sym->offset);
          });
        } else if (!def.init) {
          // SysY 评测通常假定未赋初值的局部标量为 0（与官方运行时一致）
          if (sym->base == BaseType::Float) {
            emit("\tfmv.w.x\tfa0, zero");
          } else {
            emit("\tli\ta0, 0");
          }
          emitStoreToAddress(sym->base, [this, sym]() {
            emitAddOffset("a1", "s0", sym->offset);
          });
        }
        continue;
      }
      int total = product(sym->dims);
      if (!def.init) {
        string zeroLoop = newLabel("zero_array");
        string zeroEnd = newLabel("zero_array_end");
        emitAddOffset("t0", "s0", sym->offset);
        emit("\tli\tt1, " + to_string(total * 4));
        emit(zeroLoop + ":");
        emit("\tbeqz\tt1, " + zeroEnd);
        emit("\tsw\tzero, 0(t0)");
        emit("\taddi\tt0, t0, 4");
        emit("\taddi\tt1, t1, -4");
        emit("\tj\t" + zeroLoop);
        emit(zeroEnd + ":");
        continue;
      }
      string zeroLoop = newLabel("zero_array");
      string zeroEnd = newLabel("zero_array_end");
      emitAddOffset("t0", "s0", sym->offset);
      emit("\tli\tt1, " + to_string(total * 4));
      emit(zeroLoop + ":");
      emit("\tbeqz\tt1, " + zeroEnd);
      emit("\tsw\tzero, 0(t0)");
      emit("\taddi\tt0, t0, 4");
      emit("\taddi\tt1, t1, -4");
      emit("\tj\t" + zeroLoop);
      emit(zeroEnd + ":");
      vector<InitVal *> flat(total, nullptr);
      flattenRuntimeInit(def.init.get(), sym->dims, 0, 0, flat);
      for (int i = 0; i < total; ++i) {
        if (!flat[i]) {
          continue;
        }
        emitExpr(flat[i]->expr.get());
        emitConvert(flat[i]->expr->type, Type::scalar(sym->base));
        emitStoreToAddress(sym->base, [this, sym, i]() {
          emitAddOffset("a1", "s0", sym->offset + i * 4);
        });
      }
    }
  }

int CodeGen::flattenRuntimeInit(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<InitVal *> &flat) {
    if (!init->isList) {
      if (start < static_cast<int>(flat.size())) {
        flat[start] = init;
      }
      return start + 1;
    }
    int subSize = depth >= dims.size() ? 1 : product(dims, depth);
    if (init->list.empty()) {
      return start + subSize;
    }
    int idx = start;
    for (auto &child : init->list) {
      if (child->isList) {
        size_t childDepth = chooseRuntimeInitChildDepth(dims, depth, idx);
        idx = flattenRuntimeInit(child.get(), dims, childDepth, idx, flat);
      } else {
        idx = flattenRuntimeInit(child.get(), dims, dims.size(), idx, flat);
      }
    }
    return start + subSize;
  }

size_t CodeGen::chooseRuntimeInitChildDepth(const vector<int> &dims, size_t depth,
                                     int flatIndex) {
    size_t childDepth = min(depth + 1, dims.size());
    while (childDepth < dims.size()) {
      int childSize = product(dims, childDepth);
      if (childSize == 0 || flatIndex % childSize == 0) {
        break;
      }
      ++childDepth;
    }
    return childDepth;
  }

template <class AddressEmitter>
void CodeGen::emitStoreToAddress(BaseType base, AddressEmitter emitAddressToA1) {
    if (base == BaseType::Float) {
      emitPushFloat("fa0");
      emitAddressToA1();
      emitPopFloat("fa0");
      emit("\tfsw\tfa0, 0(a1)");
    } else {
      emitPushInt("a0");
      emitAddressToA1();
      emitPopInt("a0");
      emit("\tsw\ta0, 0(a1)");
    }
  }

void CodeGen::emitAssign(AssignStmt &stmt) {
    emitLValAddress(stmt.lhs.get());
    emitPushInt("a0");
    emitExpr(stmt.rhs.get());
    emitConvert(stmt.rhs->type, Type::scalar(stmt.lhs->symbol->base));
    if (stmt.lhs->symbol->base == BaseType::Float) {
      emitPopInt("a1");
      emit("\tfsw\tfa0, 0(a1)");
    } else {
      emitPushInt("a0");
      emitPopInt("t0");
      emitPopInt("a1");
      emit("\tsw\tt0, 0(a1)");
    }
  }

void CodeGen::emitIf(IfStmt &stmt) {
    string thenLabel = newLabel("if_then");
    string elseLabel = newLabel("if_else");
    string endLabel = newLabel("if_end");
    emitCond(stmt.cond.get(), thenLabel, stmt.elseStmt ? elseLabel : endLabel);
    emit(thenLabel + ":");
    emitStmt(stmt.thenStmt.get());
    emit("\tj\t" + endLabel);
    if (stmt.elseStmt) {
      emit(elseLabel + ":");
      emitStmt(stmt.elseStmt.get());
    }
    emit(endLabel + ":");
  }

void CodeGen::emitWhile(WhileStmt &stmt) {
    string condLabel = newLabel("while_cond");
    string bodyLabel = newLabel("while_body");
    string endLabel = newLabel("while_end");
    continueLabels_.push_back(condLabel);
    breakLabels_.push_back(endLabel);
    emit(condLabel + ":");
    emitCond(stmt.cond.get(), bodyLabel, endLabel);
    emit(bodyLabel + ":");
    emitStmt(stmt.body.get());
    emit("\tj\t" + condLabel);
    emit(endLabel + ":");
    breakLabels_.pop_back();
    continueLabels_.pop_back();
  }

void CodeGen::emitReturn(ReturnStmt &stmt) {
    if (stmt.expr) {
      emitExpr(stmt.expr.get());
      emitConvert(stmt.expr->type, Type::scalar(currentFunction_->ret));
    }
    emit("\tj\t" + currentFunction_->returnLabel);
  }

void CodeGen::emitExpr(Expr *expr) {
    if (expr->isConst && expr->kind != ExprKind::LVal) {
      emitConst(expr->constVal);
      return;
    }
    switch (expr->kind) {
    case ExprKind::Number:
      emitConst(expr->constVal);
      break;
    case ExprKind::String:
      emitStringExpr(static_cast<StringExpr *>(expr));
      break;
    case ExprKind::LVal:
      emitLValValue(static_cast<LValExpr *>(expr));
      break;
    case ExprKind::Call:
      emitCall(static_cast<CallExpr *>(expr));
      break;
    case ExprKind::Unary:
      emitUnary(static_cast<UnaryExpr *>(expr));
      break;
    case ExprKind::Binary:
      emitBinary(static_cast<BinaryExpr *>(expr));
      break;
    }
  }

void CodeGen::emitConst(ConstValue v) {
    if (v.type == BaseType::Float) {
      emitFloatConst(v.f);
    } else {
      emit("\tli\ta0, " + to_string(v.i));
    }
  }

void CodeGen::emitFloatConst(float value) {
    uint32_t bits = floatBits(value);
    if (bits == 0) {
      emit("\tfmv.w.x\tfa0, zero");
      return;
    }
    string label = newLabel("float");
    floatLiterals_.push_back({label, value});
    emit("\tlla\tt0, " + label);
    emit("\tflw\tfa0, 0(t0)");
  }

void CodeGen::emitStringExpr(StringExpr *expr) {
    if (expr->label.empty()) {
      expr->label = newLabel("str");
      stringLiterals_.push_back({expr->label, expr->value});
    }
    emit("\tlla\ta0, " + expr->label);
  }

void CodeGen::emitUnary(UnaryExpr *expr) {
    emitExpr(expr->expr.get());
    if (expr->op == "+") {
      return;
    }
    if (expr->op == "-") {
      if (expr->type.base == BaseType::Float) {
        emit("\tfneg.s\tfa0, fa0");
      } else {
        emit("\tnegw\ta0, a0");
      }
      return;
    }
    if (expr->op == "!") {
      emitBoolFromValue(expr->expr->type);
      emit("\tseqz\ta0, a0");
    }
  }

void CodeGen::emitBinary(BinaryExpr *expr) {
    const string &op = expr->op;
    if (op == "&&" || op == "||") {
      string trueLabel = newLabel("logic_true");
      string falseLabel = newLabel("logic_false");
      string endLabel = newLabel("logic_end");
      emitCond(expr, trueLabel, falseLabel);
      emit(trueLabel + ":");
      emit("\tli\ta0, 1");
      emit("\tj\t" + endLabel);
      emit(falseLabel + ":");
      emit("\tli\ta0, 0");
      emit(endLabel + ":");
      return;
    }

    bool resultFloat = expr->type.base == BaseType::Float;
    bool compare = op == "==" || op == "!=" || op == "<" || op == ">" ||
                   op == "<=" || op == ">=";
    bool useFloat = expr->lhs->type.isFloatScalar() || expr->rhs->type.isFloatScalar();

    emitExpr(expr->lhs.get());
    if (expr->lhs->type.isFloatScalar()) {
      emitPushFloat("fa0");
    } else {
      emitPushInt("a0");
    }
    emitExpr(expr->rhs.get());

    if (useFloat) {
      if (!expr->rhs->type.isFloatScalar()) {
        emit("\tfcvt.s.w\tfa0, a0");
      }
      emitPushFloat("fa0");
      emitPopFloat("ft1");
      if (expr->lhs->type.isFloatScalar()) {
        emitPopFloat("ft0");
      } else {
        emitPopInt("t0");
        emit("\tfcvt.s.w\tft0, t0");
      }
      emit("\tfmv.s\tfa0, ft1");
      if (compare) {
        emitFloatCompare(op);
      } else if (op == "+") {
        emit("\tfadd.s\tfa0, ft0, fa0");
      } else if (op == "-") {
        emit("\tfsub.s\tfa0, ft0, fa0");
      } else if (op == "*") {
        emit("\tfmul.s\tfa0, ft0, fa0");
      } else if (op == "/") {
        emit("\tfdiv.s\tfa0, ft0, fa0");
      }
      (void)resultFloat;
      return;
    }

    emitPushInt("a0");
    emitPopInt("a1");
    emitPopInt("a0");
    if (compare) {
      emitIntCompare(op);
    } else if (op == "+") {
      emit("\taddw\ta0, a0, a1");
    } else if (op == "-") {
      emit("\tsubw\ta0, a0, a1");
    } else if (op == "*") {
      emit("\tmulw\ta0, a0, a1");
    } else if (op == "/") {
      emit("\tdivw\ta0, a0, a1");
    } else if (op == "%") {
      emit("\tremw\ta0, a0, a1");
    }
  }

void CodeGen::emitIntCompare(const string &op) {
    if (op == "==") {
      emit("\tsubw\ta0, a0, a1");
      emit("\tseqz\ta0, a0");
    } else if (op == "!=") {
      emit("\tsubw\ta0, a0, a1");
      emit("\tsnez\ta0, a0");
    } else if (op == "<") {
      emit("\tslt\ta0, a0, a1");
    } else if (op == ">") {
      emit("\tslt\ta0, a1, a0");
    } else if (op == "<=") {
      emit("\tslt\ta0, a1, a0");
      emit("\tseqz\ta0, a0");
    } else if (op == ">=") {
      emit("\tslt\ta0, a0, a1");
      emit("\tseqz\ta0, a0");
    }
  }

void CodeGen::emitFloatCompare(const string &op) {
    if (op == "==") {
      emit("\tfeq.s\ta0, ft0, fa0");
    } else if (op == "!=") {
      emit("\tfeq.s\ta0, ft0, fa0");
      emit("\tseqz\ta0, a0");
    } else if (op == "<") {
      emit("\tflt.s\ta0, ft0, fa0");
    } else if (op == ">") {
      emit("\tflt.s\ta0, fa0, ft0");
    } else if (op == "<=") {
      emit("\tfle.s\ta0, ft0, fa0");
    } else if (op == ">=") {
      emit("\tfle.s\ta0, fa0, ft0");
    }
  }

void CodeGen::emitCond(Expr *expr, const string &trueLabel, const string &falseLabel) {
    if (expr->kind == ExprKind::Binary) {
      auto *bin = static_cast<BinaryExpr *>(expr);
      if (bin->op == "&&") {
        string mid = newLabel("land_mid");
        emitCond(bin->lhs.get(), mid, falseLabel);
        emit(mid + ":");
        emitCond(bin->rhs.get(), trueLabel, falseLabel);
        return;
      }
      if (bin->op == "||") {
        string mid = newLabel("lor_mid");
        emitCond(bin->lhs.get(), trueLabel, mid);
        emit(mid + ":");
        emitCond(bin->rhs.get(), trueLabel, falseLabel);
        return;
      }
    }
    emitExpr(expr);
    emitBoolFromValue(expr->type);
    emit("\tbnez\ta0, " + trueLabel);
    emit("\tj\t" + falseLabel);
  }

void CodeGen::emitBoolFromValue(const Type &type) {
    if (type.isPointer) {
      emit("\tsnez\ta0, a0");
    } else if (type.base == BaseType::Float) {
      emit("\tfmv.w.x\tft0, zero");
      emit("\tfeq.s\ta0, fa0, ft0");
      emit("\tseqz\ta0, a0");
    } else {
      emit("\tsnez\ta0, a0");
    }
  }

void CodeGen::emitLValValue(LValExpr *expr) {
    if (expr->isConst && !expr->type.isPointer) {
      emitConst(expr->constVal);
      return;
    }
    emitLValAddress(expr);
    if (expr->type.isPointer) {
      return;
    }
    if (expr->type.base == BaseType::Float) {
      emit("\tflw\tfa0, 0(a0)");
    } else {
      emit("\tlw\ta0, 0(a0)");
    }
  }

void CodeGen::emitLValAddress(LValExpr *expr) {
    Symbol *sym = expr->symbol;
    if (sym->isGlobal) {
      emit("\tlla\ta0, " + sym->label);
    } else if (sym->isParamArray) {
      emitLoadMem("ld", "a0", "s0", sym->offset);
    } else {
      emitAddOffset("a0", "s0", sym->offset);
    }
    if (expr->indices.empty()) {
      return;
    }
    emitPushInt("a0");
    emit("\tli\ta0, 0");
    emitPushInt("a0");
    for (size_t i = 0; i < expr->indices.size(); ++i) {
      emitExpr(expr->indices[i].get());
      emitConvert(expr->indices[i]->type, Type::scalar(BaseType::Int));
      int strideBytes = strideForIndex(sym, i) * 4;
      emit("\tli\tt0, " + to_string(strideBytes));
      emit("\tmul\tt1, a0, t0");
      emit("\tld\tt0, 0(sp)");
      emit("\tadd\tt0, t0, t1");
      emit("\tsd\tt0, 0(sp)");
    }
    emitPopInt("t0");
    emitPopInt("a0");
    emit("\tadd\ta0, a0, t0");
  }

int CodeGen::strideForIndex(Symbol *sym, size_t index) {
    // 形参数组只有「除最左维外的各维」保存在 dims 中，第 i 个下标的步长为
    // product(dims, i)；与普通局部数组下标 i 的步长 product(dims, i+1) 对应同一规则。
    if (sym->isParamArray) {
      return product(sym->dims, index);
    }
    return index + 1 < sym->dims.size() ? product(sym->dims, index + 1) : 1;
  }

void CodeGen::emitCall(CallExpr *expr) {
    Function *fn = expr->function;
    vector<ParamType> params;
    vector<Expr *> args;
    if (fn->injectLineArgument) {
      params.push_back(ParamType{BaseType::Int, false, {}});
    }
    for (size_t i = 0; i < expr->args.size(); ++i) {
      args.push_back(expr->args[i].get());
      if (i < fn->params.size()) {
        params.push_back(fn->params[i]);
      } else {
        ParamType inferred;
        inferred.base = expr->args[i]->type.base;
        inferred.isArray = expr->args[i]->type.isPointer;
        params.push_back(inferred);
      }
    }

    int stackSlots = 0;
    auto locs = computeArgLocations(params, &stackSlots);
    int tempSlots = static_cast<int>(params.size());
    int stackBytes = stackSlots * 8;
    int tempBase = stackBytes;
    int area = alignTo(stackBytes + tempSlots * 8, 16);
    if (area > 0) {
      emitAdjustSp(-area);
    }

    int paramIndex = 0;
    if (fn->injectLineArgument) {
      emit("\tli\ta0, " + to_string(expr->line));
      emitStoreMem("sd", "a0", "sp", tempBase);
      ++paramIndex;
    }
    for (Expr *arg : args) {
      ParamType expected = params[paramIndex];
      emitExpr(arg);
      Type target = expected.isArray ? Type::pointer(expected.base, expected.dims)
                                     : Type::scalar(expected.base);
      if (!expected.isArray) {
        emitConvert(arg->type, target);
      }
      int off = tempBase + paramIndex * 8;
      if (target.isPointer) {
        emitStoreMem("sd", "a0", "sp", off);
      } else if (target.base == BaseType::Float) {
        emitStoreMem("fsw", "fa0", "sp", off);
      } else {
        emitStoreMem("sd", "a0", "sp", off);
      }
      ++paramIndex;
    }

    for (size_t i = 0; i < params.size(); ++i) {
      const ParamType &p = params[i];
      const ArgLoc &loc = locs[i];
      int off = tempBase + static_cast<int>(i) * 8;
      bool isFloat = !p.isArray && p.base == BaseType::Float;
      bool isPtr = p.isArray;
      if (loc.kind == ArgLoc::FloatReg) {
        emitLoadMem("flw", "fa" + to_string(loc.index), "sp", off);
      } else if (loc.kind == ArgLoc::IntReg) {
        if (isFloat) {
          emitLoadMem("lw", "a" + to_string(loc.index), "sp", off);
        } else {
          emitLoadMem("ld", "a" + to_string(loc.index), "sp", off);
        }
      } else {
        int dst = loc.index * 8;
        if (isFloat) {
          emitLoadMem("lw", "t0", "sp", off);
          emitStoreMem("sw", "t0", "sp", dst);
        } else {
          emitLoadMem("ld", "t0", "sp", off);
          emitStoreMem("sd", "t0", "sp", dst);
        }
      }
      (void)isPtr;
    }

    emit("\tcall\t" + fn->asmName);
    if (area > 0) {
      emitAdjustSp(area);
    }
  }

void CodeGen::emitConvert(const Type &from, const Type &to) {
    if (to.base == BaseType::Void || from.isPointer || to.isPointer ||
        from.base == to.base) {
      return;
    }
    if (from.base == BaseType::Int && to.base == BaseType::Float) {
      emit("\tfcvt.s.w\tfa0, a0");
    } else if (from.base == BaseType::Float && to.base == BaseType::Int) {
      emit("\tfcvt.w.s\ta0, fa0, rtz");
    }
  }

void CodeGen::emitPushInt(const string &reg) {
    emit("\taddi\tsp, sp, -16");
    emit("\tsd\t" + reg + ", 0(sp)");
  }

void CodeGen::emitPopInt(const string &reg) {
    emit("\tld\t" + reg + ", 0(sp)");
    emit("\taddi\tsp, sp, 16");
  }

void CodeGen::emitPushFloat(const string &reg) {
    emit("\taddi\tsp, sp, -16");
    emit("\tfsw\t" + reg + ", 0(sp)");
  }

void CodeGen::emitPopFloat(const string &reg) {
    emit("\tflw\t" + reg + ", 0(sp)");
    emit("\taddi\tsp, sp, 16");
  }

void CodeGen::emitLiteralPools() {
    if (floatLiterals_.empty() && stringLiterals_.empty()) {
      return;
    }
    emit("\t.section\t.rodata");
    for (const auto &lit : floatLiterals_) {
      emit("\t.align\t2");
      emit(lit.first + ":");
      emit("\t.word\t" + to_string(floatBits(lit.second)));
    }
    for (const auto &lit : stringLiterals_) {
      emit("\t.align\t1");
      emit(lit.first + ":");
      emit("\t.asciz\t\"" + escapeAsmString(lit.second) + "\"");
    }
  }
