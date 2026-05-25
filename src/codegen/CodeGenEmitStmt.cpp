// compiler2026-x phase-E (codegen statements)
// compiler2026-x phase-E (codegen)

#include "CodeGen.h"
#include "../utils/DynamicCast.h"
#include "Attrs.h"
#include "OpBase.h"
#include "Ops.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace sys;

void CodeGen::cgcEmit(ASTNode *node) {
  if (isa<EmptyNode>(node))
    return;
  
  if (auto block = dyn_cast<BlockNode>(node)) {
    SemanticScope scope(*this);
    for (auto x : block->nodes)
      cgcEmit(x);
    return;
  }

  if (auto block = dyn_cast<TransparentBlockNode>(node)) {
    for (auto x : block->nodes)
      cgcEmit(x);
    return;
  }

  if (auto fn = dyn_cast<FnDeclNode>(node)) {
    auto fnTy = cast<FunctionType>(fn->type);
    std::vector<Value::Type> argTypes;
    argTypes.reserve(fnTy->params.size());
    for (auto paramTy : fnTy->params)
      argTypes.push_back(isa<FloatType>(paramTy) ? Value::f32 : Value::i32);
    auto funcOp = builder.create<FuncOp>({
      new NameAttr(fn->name),
      new ArgCountAttr(fnTy->params.size()),
      new ArgTypesAttr(argTypes)
    });
    auto bb = funcOp->createFirstBlock();

    Builder::Guard guard(builder);
    builder.setToBlockStart(bb);

    // Function arguments are in the same scope with body.
    SemanticScope scope(*this);
    for (int i = 0; i < fn->args.size(); i++) {
      auto argTy = fnTy->params[i];
      auto size = getSize(argTy);
      // Get the value of the argument and create a temp variable for it.
      Value::Type ty = isa<FloatType>(argTy) ? Value::f32 : Value::i32;
      auto arg = builder.create<GetArgOp>(ty, { new IntAttr(i) });
      // Preserve argument reads so backend ABI lowering sees the full layout.
      arg->add<ImpureAttr>();
      if ((isa<ArrayType>(argTy) || isa<PointerType>(argTy)) && !funcOp->has<ImpureAttr>())
        funcOp->add<ImpureAttr>();

      auto addr = builder.create<AllocaOp>({ new SizeAttr(size) });
      builder.create<StoreOp>({ arg, addr }, { new SizeAttr(size) });
      // Mark address as floating point if necessary.
      if (isa<FloatType>(argTy))
        addr->add<FPAttr>();
      
      symbols[fn->args[i]] = addr;
    }

    for (auto x : fn->body->nodes)
      cgcEmit(x);
    return;
  }

  if (auto vardecl = dyn_cast<VarDeclNode>(node)) {
    if (vardecl->global) {
      if (vardecl->init && isa<IntNode>(vardecl->init)) {
        int value = cast<IntNode>(vardecl->init)->value;
        // Treat the single integer as an array.
        auto addr = builder.create<GlobalOp>({
          new SizeAttr(getSize(vardecl->type)),
          new IntArrayAttr(new int(value), 1),
          new NameAttr(vardecl->name),
          new DimensionAttr({ 1 }),
        });
        if (!vardecl->mut)
          addr->add<ConstAttr>();
        else
          addr->add<ImpureAttr>();
        globals[vardecl->name] = addr;
        return;
      }

      if (vardecl->init && isa<FloatNode>(vardecl->init)) {
        float value = cast<FloatNode>(vardecl->init)->value;
        // Treat the single integer as an array.
        auto addr = builder.create<GlobalOp>({
          new SizeAttr(getSize(vardecl->type)),
          new FloatArrayAttr(new float(value), 1),
          new NameAttr(vardecl->name),
          new DimensionAttr({ 1 }),
        });
        if (!vardecl->mut)
          addr->add<ConstAttr>();
        else
          addr->add<ImpureAttr>();
        globals[vardecl->name] = addr;
        return;
      }

      auto size = 1;
      Type *base = vardecl->type;
      auto arrTy = dyn_cast<ArrayType>(vardecl->type);
      if (arrTy) {
        size = arrTy->getSize();
        base = arrTy->base;
      }

      void *value;
      if (vardecl->init)
        value = isa<FloatType>(base)
          ? (void*) cast<ConstArrayNode>(vardecl->init)->vf
          : (void*) cast<ConstArrayNode>(vardecl->init)->vi;
      else
        value = nullptr;
      
      Value addr;
      if (isa<FloatType>(base)) {
        addr = builder.create<GlobalOp>({
          new SizeAttr(getSize(vardecl->type)),
          new FloatArrayAttr((float*) value, size),
          new NameAttr(vardecl->name),
          new DimensionAttr(arrTy ? arrTy->dims : std::vector { 1 }),
        });
        addr.defining->add<FPAttr>();
      } else {
        addr = builder.create<GlobalOp>({
          new SizeAttr(getSize(vardecl->type)),
          new IntArrayAttr((int*) value, size),
          new NameAttr(vardecl->name),
          new DimensionAttr(arrTy ? arrTy->dims : std::vector { 1 }),
        });
      }
      if (!vardecl->mut)
        addr.defining->add<ConstAttr>();
      else
        addr.defining->add<ImpureAttr>();
      globals[vardecl->name] = addr;
      return;
    }
    
    auto addr = builder.create<AllocaOp>({
      new SizeAttr(getSize(vardecl->type))
    });
    if (isa<FloatType>(vardecl->type))
      addr->add<FPAttr>();
    
    symbols[vardecl->name] = addr;
    // An uninitialized local array.
    // Give it another alloca.
    if (isa<ArrayType>(vardecl->type) && !vardecl->init) {
      auto arrayPtr = builder.create<AllocaOp>({
        new SizeAttr(8)
      });
      builder.create<StoreOp>({ addr, arrayPtr }, { new SizeAttr(8) });
      symbols[vardecl->name] = arrayPtr;
      // Also check whether this is a floating point array.
      // If it is, give the original alloca a FPAttr.
      auto arrTy = cast<ArrayType>(vardecl->type);
      addr->add<DimensionAttr>(arrTy->dims);
      if (isa<FloatType>(arrTy->base))
        addr->add<FPAttr>();
      return;
    }

    if (vardecl->init) {
      // This is a local variable with array initializer.
      // We manually load everything into the array.
      if (auto arr = dyn_cast<LocalArrayNode>(vardecl->init)) {
        auto arrTy = cast<ArrayType>(vardecl->type);
        auto base = arrTy->base;
        auto arrSize = arrTy->getSize();
        auto baseSize = getSize(arrTy->base);
        int zeroFrom = arrSize - 1;
        for (; zeroFrom >= 0; zeroFrom--) {
          if (arr->va[zeroFrom])
            break;
        }
        // Now [zeroFrom + 1, arrSize) are all zeroes.
        // We don't want to create too many stores. Create a loop instead.
        int max = arrSize - zeroFrom >= 16384 ? zeroFrom : arrSize;
        for (int i = 0; i < max; i++) {
          Value value = arr->va[i]
            ? cgcEmitExpr(arr->va[i])
            : isa<FloatType>(base)
              ? (Value) builder.create<FloatOp>({ new FloatAttr(0) })
              : (Value) builder.create<IntOp>({ new IntAttr(0) });

          auto offset =  builder.create<IntOp>({ new IntAttr(baseSize * i) });
          auto place = builder.create<AddLOp>({ addr, offset });
          builder.create<StoreOp>({ value, place }, { new SizeAttr(baseSize) });
        }
        if (max != arrSize) {
          auto start = builder.create<IntOp>({ new IntAttr(zeroFrom + 1) });
          auto end = builder.create<IntOp>({ new IntAttr(arrSize) });
          auto iv = builder.create<AllocaOp>({ new SizeAttr(4) });
          auto zero = isa<FloatType>(base)
              ? (Value) builder.create<FloatOp>({ new FloatAttr(0) })
              : (Value) builder.create<IntOp>({ new IntAttr(0) });
          auto stride = builder.create<IntOp>({ new IntAttr(baseSize) });
          auto incr = builder.create<IntOp>({ new IntAttr(1) });

          auto loop = builder.create<ForOp>({ start, end, incr, iv });
          auto body = loop->appendRegion();
          body->appendBlock();
          
          builder.setToRegionStart(body);

          auto offset = builder.create<MulIOp>({ loop, stride });
          auto place = builder.create<AddLOp>({ addr, offset });
          builder.create<StoreOp>({ zero, place });
        }

        // An extra layer of indirection is needed for further reference.
        auto arrayPtr = builder.create<AllocaOp>({
          new SizeAttr(8)
        });
        builder.create<StoreOp>({ addr, arrayPtr }, { new SizeAttr(8) });
        symbols[vardecl->name] = arrayPtr;
        addr->add<DimensionAttr>(arrTy->dims);
        // Give a FPAttr if the array is float*.
        if (isa<FloatType>(arrTy->base))
          addr->add<FPAttr>();
        return;
      }

      auto value = cgcEmitExpr(vardecl->init);
      auto store = builder.create<StoreOp>({ value, addr });
      store->add<SizeAttr>(getSize(vardecl->type));
    }
    return;
  }

  if (auto ret = dyn_cast<ReturnNode>(node)) {
    if (!ret->node) {
      builder.create<ReturnOp>();
      return;
    }
    auto value = cgcEmitExpr(ret->node);
    builder.create<ReturnOp>({ value });
    return;
  }

  if (auto branch = dyn_cast<IfNode>(node)) {
    auto cond = cgcEmitExpr(branch->cond);
    auto op = builder.create<IfOp>({ cond });

    auto thenBlock = op->createFirstBlock();
    {
      Builder::Guard guard(builder);
      builder.setToBlockStart(thenBlock);
      cgcEmit(branch->ifso);
    }
    if (branch->ifnot) {
      auto elseRegion = op->appendRegion();
      auto elseBlock = elseRegion->appendBlock();
      Builder::Guard guard(builder);
      builder.setToBlockStart(elseBlock);
      cgcEmit(branch->ifnot);
    }
    return;
  }

  if (auto loop = dyn_cast<WhileNode>(node)) {
    // Imitate the design of scf.while.
    // The `condRegion` is the `before` region, and the last op of it is ProceedOp;
    // Only when the operand of ProceedOp is true, the `after` region is executed,
    // which is called `bodyRegion` here.
    auto op = builder.create<WhileOp>();
    auto condRegion = op->createFirstBlock();

    {
      Builder::Guard guard(builder);
      builder.setToBlockStart(condRegion);
      auto cond = cgcEmitExpr(loop->cond);
      builder.create<ProceedOp>({ cond });
    }
    auto bodyRegion = op->appendRegion();
    auto bodyBlock = bodyRegion->appendBlock();

    Builder::Guard guard(builder);
    builder.setToBlockStart(bodyBlock);
    cgcEmit(loop->body);
    return;
  }
  
  if (isa<BreakNode>(node)) {
    builder.create<BreakOp>();
    return;
  }

  if (isa<ContinueNode>(node)) {
    builder.create<ContinueOp>();
    return;
  }

  if (auto assign = dyn_cast<AssignNode>(node)) {
    auto l = cast<VarRefNode>(assign->l);
    Value addr;
    if (symbols.count(l->name))
      addr = symbols[l->name];
    else if (globals.count(l->name))
      addr = builder.create<GetGlobalOp>({
        new NameAttr(l->name)
      });
    else {
      std::cerr << "assign to unknown name: " << l->name << "\n";
      assert(false);
    }
    auto value = cgcEmitExpr(assign->r);
    builder.create<StoreOp>({ value, addr }, {
      new SizeAttr(getSize(assign->l->type))
    });
    return;
  }

  if (auto write = dyn_cast<ArrayAssignNode>(node)) {
    auto value = cgcEmitExpr(write->value);
    auto arrTy = cast<ArrayType>(write->arrTy);

    // Calculate a series of stride.
    std::vector<int> sizes;
    auto size = getSize(arrTy->base) * arrTy->getSize();
    for (int i = 0; i < write->indices.size(); i++)
      sizes.push_back(size /= arrTy->dims[i]);
    
    Value addr;
    if (symbols.count(write->array))
      addr = builder.create<LoadOp>(Value::i64, {
        symbols[write->array]
      }, { new SizeAttr(8) });
    else if (globals.count(write->array))
      addr = builder.create<GetGlobalOp>({
        new NameAttr(write->array)
      });
    else {
      std::cerr << "assign to unknown name: " << write->array << "\n";
      assert(false);
    }
    for (int i = 0; i < write->indices.size(); i++) {
      auto index = cgcEmitExpr(write->indices[i]);
      auto strideVal = builder.create<IntOp>({ new IntAttr(sizes[i]) });
      auto stride = builder.create<MulIOp>({ index, strideVal });
      addr = builder.create<AddLOp>({ addr, stride });
    }
    // Store the value in addr.
    builder.create<StoreOp>({ value, addr }, {
      new SizeAttr(getSize(arrTy->base))
    });
    return;
  }
  
  cgcEmitExpr(node);
}
