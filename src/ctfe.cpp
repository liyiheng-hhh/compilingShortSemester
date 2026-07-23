#include "ctfe.hpp"

#include <chrono>
#include <cstdlib>
#include <deque>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace toyc {
namespace {

class CtfeAbort final {};

class WholeProgramEvaluator {
public:
    explicit WholeProgramEvaluator(CompUnit& unit)
        : unit_(unit), start_(std::chrono::steady_clock::now()) {}

    std::optional<std::int32_t> evaluate() {
        try {
            prepareBindings();
            for (const auto& decl : unit_.globals) {
                const std::int32_t value = evalExpr(*decl->init, nullptr);
                const std::size_t slot = checkedSlot(decl->ctfeSlot, globals_.size());
                globals_[slot] = Cell{value, decl->isConst, true};
            }

            const auto main = functions_.find("main");
            if (main == functions_.end() || main->second->returnType != ValueType::Int ||
                !main->second->params.empty()) {
                return std::nullopt;
            }
            return invoke(*main->second, {});
        } catch (const CtfeAbort&) {
            diagnoseAbort();
            return std::nullopt;
        }
    }

private:
    // Platform compile limits punish long CTFE. Use a hard 8s cap, but abort the
    // slow AST/bytecode interpreter after 1.5s so only affine/dense fast paths
    // may spend the remaining budget.
    static constexpr std::uint64_t kStepLimit = 4'000'000'000;
    static constexpr int kCallDepthLimit = 2048;
    static constexpr std::size_t kMemoEntryLimit = 500'000;
    static constexpr int kCtfeTimeLimitSeconds = 8;
    static constexpr int kCtfeSlowPathLimitMilliseconds = 1500;
    static constexpr std::size_t kAffineDimensionLimit = 40;
    static constexpr std::uint64_t kAffineMinIterations = 16;
    static constexpr std::uint64_t kLoopTickBatch = 64;
    // Sample the wall clock far less often than step accounting.
    static constexpr std::uint64_t kLoopClockBatch = 65536;

    struct Cell {
        std::int32_t value = 0;
        bool isConst = false;
        bool initialized = false;
    };

    struct Binding {
        int slot = -1;
        bool global = false;
    };

    struct Frame {
        const Function* function = nullptr;
        std::vector<Cell> locals;
        std::vector<std::int32_t> tailArgs;
    };

    enum class FlowKind {
        Normal,
        Break,
        Continue,
        Return,
        TailCall,
    };

    struct Flow {
        FlowKind kind = FlowKind::Normal;
        std::int32_t value = 0;
    };

    struct EffectInfo {
        bool touchesGlobal = false;
        int selfCallSites = 0;
        std::vector<const Function*> callees;
    };

    struct ArgsHash {
        std::size_t operator()(const std::vector<std::int32_t>& args) const noexcept {
            std::size_t result = args.size();
            for (const std::int32_t value : args) {
                const std::size_t item = std::hash<std::int32_t>{}(value);
                result ^= item + 0x9e3779b9u + (result << 6) + (result >> 2);
            }
            return result;
        }
    };

    using MemoTable = std::unordered_map<std::vector<std::int32_t>, std::int32_t, ArgsHash>;

    CompUnit& unit_;
    std::unordered_map<std::string, Function*> functions_;
    std::vector<Cell> globals_;
    std::unordered_map<std::string, Binding> globalBindings_;
    std::vector<std::unordered_map<std::string, Binding>> bindingScopes_;
    std::unordered_map<const Function*, MemoTable> memo_;
    std::deque<Frame> framePool_;
    std::uint64_t steps_ = 0;
    int callDepth_ = 0;
    int fastLoopDepth_ = 0;
    std::chrono::steady_clock::time_point start_;

    void checkTimeLimit() const {
        const auto elapsed = std::chrono::steady_clock::now() - start_;
        if (elapsed > std::chrono::seconds(kCtfeTimeLimitSeconds)) {
            throw CtfeAbort{};
        }
        if (fastLoopDepth_ == 0 &&
            elapsed > std::chrono::milliseconds(kCtfeSlowPathLimitMilliseconds)) {
            throw CtfeAbort{};
        }
    }

    struct FastLoopGuard {
        int& depth;
        explicit FastLoopGuard(int& d) : depth(d) { ++depth; }
        ~FastLoopGuard() { --depth; }
        FastLoopGuard(const FastLoopGuard&) = delete;
        FastLoopGuard& operator=(const FastLoopGuard&) = delete;
    };

    void tick() {
        ++steps_;
        if (steps_ > kStepLimit) {
            throw CtfeAbort{};
        }
        if ((steps_ & (kLoopClockBatch - 1)) == 0) {
            checkTimeLimit();
        }
    }

    void tickMany(std::uint64_t count) {
        steps_ += count;
        if (steps_ > kStepLimit) {
            throw CtfeAbort{};
        }
        // Dense/bytecode loops call this often; only sample the clock periodically.
        if ((steps_ & (kLoopClockBatch - 1)) < count) {
            checkTimeLimit();
        }
    }

    void diagnoseAbort() const {
        const char* flag = std::getenv("TOYC_CTFE_DIAG");
        if (!flag || !*flag || flag[0] == '0') {
            return;
        }
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_);
        std::cerr << "toyc: ctfe aborted after " << steps_ << " steps, depth "
                  << callDepth_ << ", " << elapsed.count() << " ms\n";
    }

    static std::size_t checkedSlot(int slot, std::size_t size) {
        if (slot < 0 || static_cast<std::size_t>(slot) >= size) {
            throw CtfeAbort{};
        }
        return static_cast<std::size_t>(slot);
    }

    Binding resolveBinding(const std::string& name) const {
        for (auto scope = bindingScopes_.rbegin(); scope != bindingScopes_.rend(); ++scope) {
            const auto found = scope->find(name);
            if (found != scope->end()) {
                return found->second;
            }
        }
        const auto global = globalBindings_.find(name);
        if (global != globalBindings_.end()) {
            return global->second;
        }
        throw CtfeAbort{};
    }

    void prepareBindings() {
        functions_.clear();
        for (const auto& function : unit_.functions) {
            functions_[function->name] = function.get();
        }

        globals_.assign(unit_.globals.size(), {});
        globalBindings_.clear();
        bindingScopes_.clear();

        for (std::size_t i = 0; i < unit_.globals.size(); ++i) {
            Decl& decl = *unit_.globals[i];
            bindExpr(*decl.init);
            decl.ctfeSlot = static_cast<int>(i);
            decl.ctfeGlobal = true;
            globalBindings_[decl.name] = Binding{static_cast<int>(i), true};
        }

        for (const auto& function : unit_.functions) {
            bindFunction(*function);
            compileFunction(*function);
        }
        analyzeMemoizableFunctions();
        memo_.clear();
        framePool_.clear();
    }

    void bindFunction(Function& function) {
        bindingScopes_.clear();
        bindingScopes_.emplace_back();

        int nextSlot = 0;
        for (const std::string& param : function.params) {
            bindingScopes_.back()[param] = Binding{nextSlot++, false};
        }
        bindStmt(*function.body, nextSlot);
        function.ctfeLocalCount = static_cast<std::size_t>(nextSlot);
        bindingScopes_.clear();
    }

    void bindStmt(Stmt& stmt, int& nextSlot) {
        switch (stmt.kind) {
            case StmtKind::Block:
                bindingScopes_.emplace_back();
                for (const auto& child : stmt.statements) {
                    bindStmt(*child, nextSlot);
                }
                bindingScopes_.pop_back();
                return;
            case StmtKind::Empty:
            case StmtKind::Break:
            case StmtKind::Continue:
                return;
            case StmtKind::ExprStmt:
            case StmtKind::Return:
                if (stmt.expr) {
                    bindExpr(*stmt.expr);
                }
                return;
            case StmtKind::Assign: {
                bindExpr(*stmt.expr);
                const Binding binding = resolveBinding(stmt.name);
                stmt.ctfeSlot = binding.slot;
                stmt.ctfeGlobal = binding.global;
                return;
            }
            case StmtKind::DeclStmt: {
                Decl& decl = *stmt.decl;
                bindExpr(*decl.init);
                decl.ctfeSlot = nextSlot++;
                decl.ctfeGlobal = false;
                if (bindingScopes_.empty()) {
                    throw CtfeAbort{};
                }
                bindingScopes_.back()[decl.name] = Binding{decl.ctfeSlot, false};
                return;
            }
            case StmtKind::If:
                bindExpr(*stmt.expr);
                bindStmt(*stmt.thenBranch, nextSlot);
                if (stmt.elseBranch) {
                    bindStmt(*stmt.elseBranch, nextSlot);
                }
                return;
            case StmtKind::While:
                bindExpr(*stmt.expr);
                bindStmt(*stmt.thenBranch, nextSlot);
                return;
        }
        throw CtfeAbort{};
    }

    void bindExpr(Expr& expr) {
        switch (expr.kind) {
            case ExprKind::Number:
                return;
            case ExprKind::Variable: {
                const Binding binding = resolveBinding(expr.name);
                expr.ctfeSlot = binding.slot;
                expr.ctfeGlobal = binding.global;
                return;
            }
            case ExprKind::Unary:
                bindExpr(*expr.lhs);
                expr.ctfeOp = decodeUnaryOp(expr.op);
                return;
            case ExprKind::Binary:
                bindExpr(*expr.lhs);
                bindExpr(*expr.rhs);
                expr.ctfeOp = decodeBinaryOp(expr.op);
                return;
            case ExprKind::Call: {
                for (const auto& arg : expr.args) {
                    bindExpr(*arg);
                }
                const auto function = functions_.find(expr.name);
                if (function == functions_.end()) {
                    throw CtfeAbort{};
                }
                expr.ctfeCallee = function->second;
                return;
            }
        }
        throw CtfeAbort{};
    }

    static CtfeOp decodeUnaryOp(const std::string& op) {
        if (op == "+") {
            return CtfeOp::Positive;
        }
        if (op == "-") {
            return CtfeOp::Negative;
        }
        if (op == "!") {
            return CtfeOp::LogicalNot;
        }
        throw CtfeAbort{};
    }

    static CtfeOp decodeBinaryOp(const std::string& op) {
        if (op == "+") return CtfeOp::Add;
        if (op == "-") return CtfeOp::Subtract;
        if (op == "*") return CtfeOp::Multiply;
        if (op == "/") return CtfeOp::Divide;
        if (op == "%") return CtfeOp::Remainder;
        if (op == "<") return CtfeOp::Less;
        if (op == ">") return CtfeOp::Greater;
        if (op == "<=") return CtfeOp::LessEqual;
        if (op == ">=") return CtfeOp::GreaterEqual;
        if (op == "==") return CtfeOp::Equal;
        if (op == "!=") return CtfeOp::NotEqual;
        if (op == "&&") return CtfeOp::LogicalAnd;
        if (op == "||") return CtfeOp::LogicalOr;
        throw CtfeAbort{};
    }

    bool isMutableGlobalSlot(int slot) const {
        if (slot < 0 || static_cast<std::size_t>(slot) >= unit_.globals.size()) {
            return true;
        }
        return !unit_.globals[static_cast<std::size_t>(slot)]->isConst;
    }

    void collectExprEffects(const Expr& expr, const Function& owner, EffectInfo& info) const {
        switch (expr.kind) {
            case ExprKind::Number:
                return;
            case ExprKind::Variable:
                // Reading a global const is referentially transparent for memoization.
                info.touchesGlobal =
                    info.touchesGlobal ||
                    (expr.ctfeGlobal && isMutableGlobalSlot(expr.ctfeSlot));
                return;
            case ExprKind::Unary:
                collectExprEffects(*expr.lhs, owner, info);
                return;
            case ExprKind::Binary:
                collectExprEffects(*expr.lhs, owner, info);
                collectExprEffects(*expr.rhs, owner, info);
                return;
            case ExprKind::Call:
                if (!expr.ctfeCallee) {
                    throw CtfeAbort{};
                }
                info.callees.push_back(expr.ctfeCallee);
                if (expr.ctfeCallee == &owner) {
                    ++info.selfCallSites;
                }
                for (const auto& arg : expr.args) {
                    collectExprEffects(*arg, owner, info);
                }
                return;
        }
        throw CtfeAbort{};
    }

    void collectStmtEffects(const Stmt& stmt, const Function& owner, EffectInfo& info) const {
        switch (stmt.kind) {
            case StmtKind::Block:
                for (const auto& child : stmt.statements) {
                    collectStmtEffects(*child, owner, info);
                }
                return;
            case StmtKind::Empty:
            case StmtKind::Break:
            case StmtKind::Continue:
                return;
            case StmtKind::ExprStmt:
            case StmtKind::Return:
                if (stmt.expr) {
                    collectExprEffects(*stmt.expr, owner, info);
                }
                return;
            case StmtKind::Assign:
                info.touchesGlobal = info.touchesGlobal || stmt.ctfeGlobal;
                collectExprEffects(*stmt.expr, owner, info);
                return;
            case StmtKind::DeclStmt:
                collectExprEffects(*stmt.decl->init, owner, info);
                return;
            case StmtKind::If:
                collectExprEffects(*stmt.expr, owner, info);
                collectStmtEffects(*stmt.thenBranch, owner, info);
                if (stmt.elseBranch) {
                    collectStmtEffects(*stmt.elseBranch, owner, info);
                }
                return;
            case StmtKind::While:
                collectExprEffects(*stmt.expr, owner, info);
                collectStmtEffects(*stmt.thenBranch, owner, info);
                return;
        }
        throw CtfeAbort{};
    }

    void analyzeMemoizableFunctions() {
        std::unordered_map<const Function*, EffectInfo> effects;
        for (const auto& function : unit_.functions) {
            EffectInfo info;
            collectStmtEffects(*function->body, *function, info);
            function->ctfeMemoizable = !info.touchesGlobal;
            effects.emplace(function.get(), std::move(info));
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& function : unit_.functions) {
                if (!function->ctfeMemoizable) {
                    continue;
                }
                for (const Function* callee : effects.at(function.get()).callees) {
                    if (!callee->ctfeMemoizable) {
                        function->ctfeMemoizable = false;
                        changed = true;
                        break;
                    }
                }
            }
        }

        for (const auto& function : unit_.functions) {
            const EffectInfo& info = effects.at(function.get());
            // Memoize tree-recursive pure functions. Single self-call sites are usually
            // linear/tail recursion where a memo table only adds overhead.
            function->ctfeMemoizable = function->ctfeMemoizable &&
                                         function->returnType == ValueType::Int &&
                                         info.selfCallSites >= 2;
        }
    }

    Cell* boundCell(Frame* frame, int slot, bool global, bool requireInitialized = true) {
        if (global) {
            const std::size_t index = checkedSlot(slot, globals_.size());
            Cell* cell = &globals_[index];
            if (requireInitialized && !cell->initialized) {
                throw CtfeAbort{};
            }
            return cell;
        }
        if (!frame) {
            throw CtfeAbort{};
        }
        Cell* cell = &frame->locals[checkedSlot(slot, frame->locals.size())];
        if (requireInitialized && !cell->initialized) {
            throw CtfeAbort{};
        }
        return cell;
    }

    // Hot-path cell access: slots are bound during prepareBindings.
    std::int32_t readSlot(Frame* frame, int slot, bool global) const {
        if (global) {
            if (slot < 0 || static_cast<std::size_t>(slot) >= globals_.size() ||
                !globals_[static_cast<std::size_t>(slot)].initialized) {
                throw CtfeAbort{};
            }
            return globals_[static_cast<std::size_t>(slot)].value;
        }
        if (!frame || slot < 0 ||
            static_cast<std::size_t>(slot) >= frame->locals.size() ||
            !frame->locals[static_cast<std::size_t>(slot)].initialized) {
            throw CtfeAbort{};
        }
        return frame->locals[static_cast<std::size_t>(slot)].value;
    }

    void writeSlot(Frame* frame, int slot, bool global, std::int32_t value) {
        if (global) {
            if (slot < 0 || static_cast<std::size_t>(slot) >= globals_.size()) {
                throw CtfeAbort{};
            }
            Cell& cell = globals_[static_cast<std::size_t>(slot)];
            if (cell.isConst) {
                throw CtfeAbort{};
            }
            cell.value = value;
            cell.initialized = true;
            return;
        }
        if (!frame || slot < 0 ||
            static_cast<std::size_t>(slot) >= frame->locals.size()) {
            throw CtfeAbort{};
        }
        Cell& cell = frame->locals[static_cast<std::size_t>(slot)];
        if (cell.isConst) {
            throw CtfeAbort{};
        }
        cell.value = value;
        cell.initialized = true;
    }

    enum BcOp : std::int32_t {
        BcLoadImm = 1,
        BcLoadLocal,
        BcLoadGlobal,
        BcStoreLocal,
        BcStoreLocalConst,
        BcStoreGlobal,
        BcAdd,
        BcSub,
        BcMul,
        BcDiv,
        BcRem,
        BcLt,
        BcGt,
        BcLe,
        BcGe,
        BcEq,
        BcNe,
        BcNeg,
        BcNot,
        BcJump,
        BcJumpIf0,
        BcJumpIfNot0,
        BcCall,
        BcReturn,
        BcReturn0,
        BcTailSelf,
        BcTickBatch,
        BcAffineWhile,
        BcPop,
    };

    void emitOp(Function& function, BcOp op) { function.ctfeCode.push_back(op); }

    void emitOp(Function& function, BcOp op, std::int32_t a) {
        function.ctfeCode.push_back(op);
        function.ctfeCode.push_back(a);
    }

    void emitOp(Function& function, BcOp op, std::int32_t a, std::int32_t b) {
        function.ctfeCode.push_back(op);
        function.ctfeCode.push_back(a);
        function.ctfeCode.push_back(b);
    }

    std::size_t emitJumpPlaceholder(Function& function, BcOp op) {
        emitOp(function, op, 0);
        return function.ctfeCode.size() - 1;
    }

    void patchJump(Function& function, std::size_t operandIndex, std::size_t target) {
        function.ctfeCode[operandIndex] = static_cast<std::int32_t>(target);
    }

    int functionIndex(const Function* function) const {
        for (std::size_t i = 0; i < unit_.functions.size(); ++i) {
            if (unit_.functions[i].get() == function) {
                return static_cast<int>(i);
            }
        }
        throw CtfeAbort{};
    }

    void compileExpr(Function& function, const Expr& expr) {
        switch (expr.kind) {
            case ExprKind::Number:
                emitOp(function, BcLoadImm, toInt32(expr.number));
                return;
            case ExprKind::Variable:
                emitOp(function, expr.ctfeGlobal ? BcLoadGlobal : BcLoadLocal, expr.ctfeSlot);
                return;
            case ExprKind::Unary:
                compileExpr(function, *expr.lhs);
                if (expr.ctfeOp == CtfeOp::Positive) {
                    return;
                }
                if (expr.ctfeOp == CtfeOp::Negative) {
                    emitOp(function, BcNeg);
                    return;
                }
                if (expr.ctfeOp == CtfeOp::LogicalNot) {
                    emitOp(function, BcNot);
                    return;
                }
                throw CtfeAbort{};
            case ExprKind::Binary:
                if (expr.ctfeOp == CtfeOp::LogicalAnd) {
                    compileExpr(function, *expr.lhs);
                    const std::size_t falseJump = emitJumpPlaceholder(function, BcJumpIf0);
                    compileExpr(function, *expr.rhs);
                    emitOp(function, BcNot);
                    emitOp(function, BcNot);
                    const std::size_t endJump = emitJumpPlaceholder(function, BcJump);
                    patchJump(function, falseJump, function.ctfeCode.size());
                    emitOp(function, BcLoadImm, 0);
                    patchJump(function, endJump, function.ctfeCode.size());
                    return;
                }
                if (expr.ctfeOp == CtfeOp::LogicalOr) {
                    compileExpr(function, *expr.lhs);
                    const std::size_t trueJump = emitJumpPlaceholder(function, BcJumpIfNot0);
                    compileExpr(function, *expr.rhs);
                    emitOp(function, BcNot);
                    emitOp(function, BcNot);
                    const std::size_t endJump = emitJumpPlaceholder(function, BcJump);
                    patchJump(function, trueJump, function.ctfeCode.size());
                    emitOp(function, BcLoadImm, 1);
                    patchJump(function, endJump, function.ctfeCode.size());
                    return;
                }
                compileExpr(function, *expr.lhs);
                compileExpr(function, *expr.rhs);
                switch (expr.ctfeOp) {
                    case CtfeOp::Add:
                        emitOp(function, BcAdd);
                        return;
                    case CtfeOp::Subtract:
                        emitOp(function, BcSub);
                        return;
                    case CtfeOp::Multiply:
                        emitOp(function, BcMul);
                        return;
                    case CtfeOp::Divide:
                        emitOp(function, BcDiv);
                        return;
                    case CtfeOp::Remainder:
                        emitOp(function, BcRem);
                        return;
                    case CtfeOp::Less:
                        emitOp(function, BcLt);
                        return;
                    case CtfeOp::Greater:
                        emitOp(function, BcGt);
                        return;
                    case CtfeOp::LessEqual:
                        emitOp(function, BcLe);
                        return;
                    case CtfeOp::GreaterEqual:
                        emitOp(function, BcGe);
                        return;
                    case CtfeOp::Equal:
                        emitOp(function, BcEq);
                        return;
                    case CtfeOp::NotEqual:
                        emitOp(function, BcNe);
                        return;
                    default:
                        throw CtfeAbort{};
                }
            case ExprKind::Call:
                for (const auto& arg : expr.args) {
                    compileExpr(function, *arg);
                }
                if (!expr.ctfeCallee) {
                    throw CtfeAbort{};
                }
                emitOp(function, BcCall, functionIndex(expr.ctfeCallee),
                       static_cast<std::int32_t>(expr.args.size()));
                return;
        }
        throw CtfeAbort{};
    }

    struct LoopEmit {
        std::size_t continueTarget = 0;
        std::vector<std::size_t> breakOperands;
    };

    void compileStmt(Function& function, const Stmt& stmt, std::vector<LoopEmit>& loops) {
        switch (stmt.kind) {
            case StmtKind::Block:
                for (const auto& child : stmt.statements) {
                    compileStmt(function, *child, loops);
                }
                return;
            case StmtKind::Empty:
                return;
            case StmtKind::ExprStmt:
                compileExpr(function, *stmt.expr);
                emitOp(function, BcPop);
                return;
            case StmtKind::Assign:
                compileExpr(function, *stmt.expr);
                emitOp(function, stmt.ctfeGlobal ? BcStoreGlobal : BcStoreLocal, stmt.ctfeSlot);
                return;
            case StmtKind::DeclStmt:
                compileExpr(function, *stmt.decl->init);
                emitOp(function,
                       stmt.decl->isConst ? BcStoreLocalConst : BcStoreLocal,
                       stmt.decl->ctfeSlot);
                return;
            case StmtKind::If: {
                compileExpr(function, *stmt.expr);
                const std::size_t elseJump = emitJumpPlaceholder(function, BcJumpIf0);
                compileStmt(function, *stmt.thenBranch, loops);
                if (!stmt.elseBranch) {
                    patchJump(function, elseJump, function.ctfeCode.size());
                    return;
                }
                const std::size_t endJump = emitJumpPlaceholder(function, BcJump);
                patchJump(function, elseJump, function.ctfeCode.size());
                compileStmt(function, *stmt.elseBranch, loops);
                patchJump(function, endJump, function.ctfeCode.size());
                return;
            }
            case StmtKind::While: {
                const std::size_t whileIndex = function.ctfeWhileStmts.size();
                function.ctfeWhileStmts.push_back(&stmt);
                emitOp(function, BcAffineWhile, static_cast<std::int32_t>(whileIndex), 0);
                const std::size_t affineEndOperand = function.ctfeCode.size() - 1;

                const std::size_t head = function.ctfeCode.size();
                emitOp(function, BcTickBatch);
                compileExpr(function, *stmt.expr);
                const std::size_t exitJump = emitJumpPlaceholder(function, BcJumpIf0);

                loops.push_back(LoopEmit{head, {}});
                compileStmt(function, *stmt.thenBranch, loops);
                emitOp(function, BcJump, static_cast<std::int32_t>(head));

                const std::size_t end = function.ctfeCode.size();
                patchJump(function, exitJump, end);
                patchJump(function, affineEndOperand, end);
                for (const std::size_t breakOperand : loops.back().breakOperands) {
                    patchJump(function, breakOperand, end);
                }
                loops.pop_back();
                return;
            }
            case StmtKind::Break:
                if (loops.empty()) {
                    throw CtfeAbort{};
                }
                loops.back().breakOperands.push_back(
                    emitJumpPlaceholder(function, BcJump));
                return;
            case StmtKind::Continue:
                if (loops.empty()) {
                    throw CtfeAbort{};
                }
                emitOp(function, BcJump,
                       static_cast<std::int32_t>(loops.back().continueTarget));
                return;
            case StmtKind::Return:
                if (!stmt.expr) {
                    emitOp(function, BcReturn0);
                    return;
                }
                if (stmt.expr->kind == ExprKind::Call &&
                    stmt.expr->ctfeCallee == &function) {
                    for (const auto& arg : stmt.expr->args) {
                        compileExpr(function, *arg);
                    }
                    emitOp(function, BcTailSelf,
                           static_cast<std::int32_t>(stmt.expr->args.size()));
                    return;
                }
                compileExpr(function, *stmt.expr);
                emitOp(function, BcReturn);
                return;
        }
        throw CtfeAbort{};
    }

    void compileFunction(Function& function) {
        function.ctfeCode.clear();
        function.ctfeWhileStmts.clear();
        std::vector<LoopEmit> loops;
        try {
            compileStmt(function, *function.body, loops);
            emitOp(function, BcReturn0);
        } catch (const CtfeAbort&) {
            function.ctfeCode.clear();
            function.ctfeWhileStmts.clear();
        }
    }

    static std::int32_t popStack(std::vector<std::int32_t>& stack) {
        if (stack.empty()) {
            throw CtfeAbort{};
        }
        const std::int32_t value = stack.back();
        stack.pop_back();
        return value;
    }

    Flow runBytecode(const Function& function, Frame& frame) {
        const std::vector<std::int32_t>& code = function.ctfeCode;
        std::vector<std::int32_t> stack;
        stack.reserve(64);
        std::size_t ip = 0;
        while (ip < code.size()) {
            const BcOp op = static_cast<BcOp>(code[ip++]);
            switch (op) {
                case BcLoadImm:
                    stack.push_back(code[ip++]);
                    break;
                case BcLoadLocal: {
                    const auto slot = static_cast<std::size_t>(code[ip++]);
                    stack.push_back(frame.locals[slot].value);
                    break;
                }
                case BcLoadGlobal:
                    stack.push_back(readSlot(&frame, code[ip++], true));
                    break;
                case BcStoreLocal: {
                    const auto slot = static_cast<std::size_t>(code[ip++]);
                    frame.locals[slot].value = popStack(stack);
                    frame.locals[slot].initialized = true;
                    break;
                }
                case BcStoreLocalConst: {
                    const int slot = code[ip++];
                    const std::int32_t value = popStack(stack);
                    Cell* cell = boundCell(&frame, slot, false, false);
                    *cell = Cell{value, true, true};
                    break;
                }
                case BcStoreGlobal:
                    writeSlot(&frame, code[ip++], true, popStack(stack));
                    break;
                case BcAdd: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) + rhs));
                    break;
                }
                case BcSub: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) - rhs));
                    break;
                }
                case BcMul: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) * rhs));
                    break;
                }
                case BcDiv: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    if (rhs == 0) {
                        throw CtfeAbort{};
                    }
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) / rhs));
                    break;
                }
                case BcRem: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    if (rhs == 0) {
                        throw CtfeAbort{};
                    }
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) % rhs));
                    break;
                }
                case BcLt: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs < rhs ? 1 : 0);
                    break;
                }
                case BcGt: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs > rhs ? 1 : 0);
                    break;
                }
                case BcLe: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs <= rhs ? 1 : 0);
                    break;
                }
                case BcGe: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs >= rhs ? 1 : 0);
                    break;
                }
                case BcEq: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs == rhs ? 1 : 0);
                    break;
                }
                case BcNe: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs != rhs ? 1 : 0);
                    break;
                }
                case BcNeg:
                    stack.push_back(toInt32(-static_cast<std::int64_t>(popStack(stack))));
                    break;
                case BcNot:
                    stack.push_back(popStack(stack) == 0 ? 1 : 0);
                    break;
                case BcJump:
                    ip = static_cast<std::size_t>(code[ip]);
                    break;
                case BcJumpIf0: {
                    const std::size_t target = static_cast<std::size_t>(code[ip++]);
                    if (popStack(stack) == 0) {
                        ip = target;
                    }
                    break;
                }
                case BcJumpIfNot0: {
                    const std::size_t target = static_cast<std::size_t>(code[ip++]);
                    if (popStack(stack) != 0) {
                        ip = target;
                    }
                    break;
                }
                case BcCall: {
                    const int calleeIndex = code[ip++];
                    const int argc = code[ip++];
                    if (calleeIndex < 0 ||
                        static_cast<std::size_t>(calleeIndex) >= unit_.functions.size() ||
                        argc < 0 || static_cast<std::size_t>(argc) > stack.size()) {
                        throw CtfeAbort{};
                    }
                    std::vector<std::int32_t> callArgs(static_cast<std::size_t>(argc));
                    for (int i = argc - 1; i >= 0; --i) {
                        callArgs[static_cast<std::size_t>(i)] = popStack(stack);
                    }
                    stack.push_back(
                        invoke(*unit_.functions[static_cast<std::size_t>(calleeIndex)],
                               std::move(callArgs)));
                    break;
                }
                case BcReturn:
                    return Flow{FlowKind::Return, popStack(stack)};
                case BcReturn0:
                    return Flow{FlowKind::Return, 0};
                case BcTailSelf: {
                    const int argc = code[ip++];
                    if (argc < 0 ||
                        static_cast<std::size_t>(argc) != function.params.size() ||
                        static_cast<std::size_t>(argc) > stack.size()) {
                        throw CtfeAbort{};
                    }
                    frame.tailArgs.resize(static_cast<std::size_t>(argc));
                    for (int i = argc - 1; i >= 0; --i) {
                        frame.tailArgs[static_cast<std::size_t>(i)] = popStack(stack);
                    }
                    return Flow{FlowKind::TailCall, 0};
                }
                case BcTickBatch:
                    // One iteration = one step. Clock is sampled every
                    // kLoopClockBatch iterations to keep large call-heavy loops
                    // from dying on host timer overhead.
                    ++steps_;
                    if (steps_ > kStepLimit) {
                        throw CtfeAbort{};
                    }
                    if ((steps_ & (kLoopClockBatch - 1)) == 0) {
                        checkTimeLimit();
                    }
                    break;
                case BcAffineWhile: {
                    const std::size_t whileIndex = static_cast<std::size_t>(code[ip++]);
                    const std::size_t end = static_cast<std::size_t>(code[ip++]);
                    if (whileIndex >= function.ctfeWhileStmts.size()) {
                        throw CtfeAbort{};
                    }
                    if (tryExecuteAffineLoop(*function.ctfeWhileStmts[whileIndex], frame)) {
                        ip = end;
                    } else if (tryExecuteNativeDenseLoop(
                                   *function.ctfeWhileStmts[whileIndex], frame)) {
                        ip = end;
                    }
                    break;
                }
                case BcPop:
                    popStack(stack);
                    break;
                default:
                    throw CtfeAbort{};
            }
        }
        throw CtfeAbort{};
    }

    std::int32_t invoke(const Function& function, std::vector<std::int32_t> args) {
        if (args.size() != function.params.size()) {
            throw CtfeAbort{};
        }

        MemoTable* memo = nullptr;
        std::vector<std::int32_t> memoKey;
        if (function.ctfeMemoizable) {
            MemoTable& table = memo_[&function];
            const auto found = table.find(args);
            if (found != table.end()) {
                return found->second;
            }
            memo = &table;
            memoKey = args;
        }

        if (callDepth_ >= kCallDepthLimit) {
            throw CtfeAbort{};
        }
        ++callDepth_;
        const std::size_t frameIndex = static_cast<std::size_t>(callDepth_ - 1);
        if (framePool_.size() <= frameIndex) {
            framePool_.emplace_back();
        }
        Frame& frame = framePool_[frameIndex];

        if (!function.ctfeCode.empty()) {
            while (true) {
                tick();
                frame.function = &function;
                frame.locals.assign(function.ctfeLocalCount, {});
                frame.tailArgs.clear();
                for (std::size_t i = 0; i < args.size(); ++i) {
                    frame.locals[i] = Cell{args[i], false, true};
                }

                Flow flow = runBytecode(function, frame);
                if (flow.kind == FlowKind::TailCall) {
                    args = std::move(frame.tailArgs);
                    if (args.size() != function.params.size()) {
                        throw CtfeAbort{};
                    }
                    continue;
                }

                --callDepth_;
                if (flow.kind == FlowKind::Return) {
                    if (memo && memo->size() < kMemoEntryLimit) {
                        memo->emplace(std::move(memoKey), flow.value);
                    }
                    return flow.value;
                }
                if (function.returnType == ValueType::Void) {
                    return 0;
                }
                throw CtfeAbort{};
            }
        }

        while (true) {
            tick();
            frame.function = &function;
            frame.locals.assign(function.ctfeLocalCount, {});
            frame.tailArgs.clear();
            for (std::size_t i = 0; i < args.size(); ++i) {
                frame.locals[i] = Cell{args[i], false, true};
            }

            Flow flow = executeStmt(*function.body, frame);
            if (flow.kind == FlowKind::TailCall) {
                args = std::move(frame.tailArgs);
                if (args.size() != function.params.size()) {
                    throw CtfeAbort{};
                }
                continue;
            }

            --callDepth_;
            if (flow.kind == FlowKind::Return) {
                if (memo && memo->size() < kMemoEntryLimit) {
                    memo->emplace(std::move(memoKey), flow.value);
                }
                return flow.value;
            }
            if (function.returnType == ValueType::Void) {
                return 0;
            }
            throw CtfeAbort{};
        }
    }

    using AffineVector = std::vector<std::uint32_t>;
    using AffineMatrix = std::vector<AffineVector>;

    std::size_t affineStateIndex(int slot, bool global, std::size_t localCount) const {
        if (global) {
            return localCount + checkedSlot(slot, globals_.size());
        }
        return checkedSlot(slot, localCount);
    }

    AffineVector affineConstant(std::uint32_t value, std::size_t dimension) const {
        AffineVector result(dimension, 0);
        result.back() = value;
        return result;
    }

    std::optional<std::uint32_t> affineConstantValue(const AffineVector& value) const {
        for (std::size_t i = 0; i + 1 < value.size(); ++i) {
            if (value[i] != 0) {
                return std::nullopt;
            }
        }
        return value.back();
    }

    bool collectAffineAssignments(const Stmt& stmt, Frame& frame,
                                  std::vector<bool>& modified,
                                  std::vector<std::int8_t>& declarations) {
        switch (stmt.kind) {
            case StmtKind::Block:
                for (const auto& child : stmt.statements) {
                    if (!collectAffineAssignments(
                            *child, frame, modified, declarations)) {
                        return false;
                    }
                }
                return true;
            case StmtKind::Empty:
                return true;
            case StmtKind::Assign: {
                const std::size_t index = affineStateIndex(
                    stmt.ctfeSlot, stmt.ctfeGlobal, frame.locals.size());
                if (declarations[index] >= 0) {
                    if (declarations[index] != 0) {
                        return false;
                    }
                } else if (boundCell(
                               &frame, stmt.ctfeSlot, stmt.ctfeGlobal)->isConst) {
                    return false;
                }
                modified[index] = true;
                return true;
            }
            case StmtKind::DeclStmt: {
                const std::size_t index = affineStateIndex(
                    stmt.decl->ctfeSlot, false, frame.locals.size());
                modified[index] = true;
                declarations[index] = stmt.decl->isConst ? 1 : 0;
                return true;
            }
            case StmtKind::ExprStmt:
            case StmtKind::If:
            case StmtKind::While:
            case StmtKind::Break:
            case StmtKind::Continue:
            case StmtKind::Return:
                return false;
        }
        return false;
    }

    std::optional<AffineVector> buildAffineExpr(
        const Expr& expr, Frame& frame, const AffineMatrix& transform,
        const std::vector<bool>& modified) {
        const std::size_t dimension = transform.size();
        switch (expr.kind) {
            case ExprKind::Number:
                return affineConstant(
                    static_cast<std::uint32_t>(toInt32(expr.number)), dimension);
            case ExprKind::Variable: {
                const std::size_t index = affineStateIndex(
                    expr.ctfeSlot, expr.ctfeGlobal, frame.locals.size());
                if (modified[index]) {
                    return transform[index];
                }
                Cell* cell = boundCell(&frame, expr.ctfeSlot, expr.ctfeGlobal);
                return affineConstant(
                    static_cast<std::uint32_t>(cell->value), dimension);
            }
            case ExprKind::Unary: {
                auto value = buildAffineExpr(*expr.lhs, frame, transform, modified);
                if (!value) {
                    return std::nullopt;
                }
                if (expr.ctfeOp == CtfeOp::Positive) {
                    return value;
                }
                if (expr.ctfeOp == CtfeOp::Negative) {
                    for (std::uint32_t& coefficient : *value) {
                        coefficient = 0u - coefficient;
                    }
                    return value;
                }
                return std::nullopt;
            }
            case ExprKind::Binary: {
                auto lhs = buildAffineExpr(*expr.lhs, frame, transform, modified);
                auto rhs = buildAffineExpr(*expr.rhs, frame, transform, modified);
                if (!lhs || !rhs) {
                    return std::nullopt;
                }
                if (expr.ctfeOp == CtfeOp::Add || expr.ctfeOp == CtfeOp::Subtract) {
                    for (std::size_t i = 0; i < dimension; ++i) {
                        if (expr.ctfeOp == CtfeOp::Add) {
                            (*lhs)[i] += (*rhs)[i];
                        } else {
                            (*lhs)[i] -= (*rhs)[i];
                        }
                    }
                    return lhs;
                }
                if (expr.ctfeOp != CtfeOp::Multiply) {
                    return std::nullopt;
                }
                const auto lhsConstant = affineConstantValue(*lhs);
                const auto rhsConstant = affineConstantValue(*rhs);
                if (!lhsConstant && !rhsConstant) {
                    return std::nullopt;
                }
                AffineVector result = lhsConstant ? std::move(*rhs) : std::move(*lhs);
                const std::uint32_t scale = lhsConstant ? *lhsConstant : *rhsConstant;
                for (std::uint32_t& coefficient : result) {
                    coefficient = static_cast<std::uint32_t>(
                        static_cast<std::uint64_t>(coefficient) * scale);
                }
                return result;
            }
            case ExprKind::Call:
                return std::nullopt;
        }
        return std::nullopt;
    }

    bool buildAffineTransform(const Stmt& stmt, Frame& frame,
                              AffineMatrix& transform,
                              const std::vector<bool>& modified) {
        switch (stmt.kind) {
            case StmtKind::Block:
                for (const auto& child : stmt.statements) {
                    if (!buildAffineTransform(*child, frame, transform, modified)) {
                        return false;
                    }
                }
                return true;
            case StmtKind::Empty:
                return true;
            case StmtKind::Assign: {
                auto value = buildAffineExpr(*stmt.expr, frame, transform, modified);
                if (!value) {
                    return false;
                }
                const std::size_t index = affineStateIndex(
                    stmt.ctfeSlot, stmt.ctfeGlobal, frame.locals.size());
                transform[index] = std::move(*value);
                return true;
            }
            case StmtKind::DeclStmt: {
                auto value = buildAffineExpr(
                    *stmt.decl->init, frame, transform, modified);
                if (!value) {
                    return false;
                }
                const std::size_t index = affineStateIndex(
                    stmt.decl->ctfeSlot, false, frame.locals.size());
                transform[index] = std::move(*value);
                return true;
            }
            case StmtKind::ExprStmt:
            case StmtKind::If:
            case StmtKind::While:
            case StmtKind::Break:
            case StmtKind::Continue:
            case StmtKind::Return:
                return false;
        }
        return false;
    }

    static AffineVector multiplyAffine(
        const AffineMatrix& matrix, const AffineVector& value) {
        AffineVector result(value.size(), 0);
        for (std::size_t i = 0; i < matrix.size(); ++i) {
            std::uint32_t sum = 0;
            for (std::size_t j = 0; j < value.size(); ++j) {
                sum += static_cast<std::uint32_t>(
                    static_cast<std::uint64_t>(matrix[i][j]) * value[j]);
            }
            result[i] = sum;
        }
        return result;
    }

    static AffineMatrix multiplyAffine(
        const AffineMatrix& lhs, const AffineMatrix& rhs) {
        const std::size_t dimension = lhs.size();
        AffineMatrix result(dimension, AffineVector(dimension, 0));
        for (std::size_t i = 0; i < dimension; ++i) {
            for (std::size_t k = 0; k < dimension; ++k) {
                if (lhs[i][k] == 0) {
                    continue;
                }
                for (std::size_t j = 0; j < dimension; ++j) {
                    result[i][j] += static_cast<std::uint32_t>(
                        static_cast<std::uint64_t>(lhs[i][k]) * rhs[k][j]);
                }
            }
        }
        return result;
    }

    static CtfeOp flipComparison(CtfeOp op) {
        switch (op) {
            case CtfeOp::Less:
                return CtfeOp::Greater;
            case CtfeOp::LessEqual:
                return CtfeOp::GreaterEqual;
            case CtfeOp::Greater:
                return CtfeOp::Less;
            case CtfeOp::GreaterEqual:
                return CtfeOp::LessEqual;
            default:
                return op;
        }
    }

    enum DenseOp : std::int32_t {
        DnLoadSlot = 1,
        DnLoadImm,
        DnStoreSlot,
        DnAdd,
        DnSub,
        DnMul,
        DnDiv,
        DnRem,
        DnNeg,
        DnNot,
        DnLt,
        DnGt,
        DnLe,
        DnGe,
        DnEq,
        DnNe,
        DnPop,
        DnJump,
        DnJumpIf0,
        DnJumpIfNot0,
    };

    static std::size_t denseSlotIndex(int slot, bool global, std::size_t localCount,
                                      std::size_t globalCount) {
        if (global) {
            return localCount + checkedSlot(slot, globalCount);
        }
        return checkedSlot(slot, localCount);
    }

    static bool tryParseInductionStep(const Expr& expr, std::size_t inductionIndex,
                                      std::size_t localCount, std::size_t globalCount,
                                      std::int32_t& step) {
        if (expr.kind != ExprKind::Binary) {
            return false;
        }
        const auto isInductionVar = [&](const Expr& value) {
            return value.kind == ExprKind::Variable &&
                   denseSlotIndex(value.ctfeSlot, value.ctfeGlobal, localCount,
                                  globalCount) == inductionIndex;
        };
        const auto tryConstant = [&](const Expr& value, std::int32_t& out) {
            if (value.kind != ExprKind::Number) {
                return false;
            }
            out = toInt32(value.number);
            return true;
        };
        if (expr.ctfeOp == CtfeOp::Add) {
            std::int32_t constant = 0;
            if (isInductionVar(*expr.lhs) && tryConstant(*expr.rhs, constant)) {
                step = constant;
                return true;
            }
            if (isInductionVar(*expr.rhs) && tryConstant(*expr.lhs, constant)) {
                step = constant;
                return true;
            }
        } else if (expr.ctfeOp == CtfeOp::Subtract) {
            std::int32_t constant = 0;
            if (isInductionVar(*expr.lhs) && tryConstant(*expr.rhs, constant)) {
                step = -constant;
                return true;
            }
        }
        return false;
    }

    bool validateDenseLoopBody(const Stmt& stmt, Frame& frame,
                               std::vector<bool>& modified,
                               std::vector<std::int8_t>& declarations,
                               std::size_t inductionIndex, int& inductionUpdates,
                               std::int32_t& step) {
        switch (stmt.kind) {
            case StmtKind::Block:
                for (const auto& child : stmt.statements) {
                    if (!validateDenseLoopBody(
                            *child, frame, modified, declarations, inductionIndex,
                            inductionUpdates, step)) {
                        return false;
                    }
                }
                return true;
            case StmtKind::Empty:
                return true;
            case StmtKind::Assign: {
                const std::size_t index = denseSlotIndex(
                    stmt.ctfeSlot, stmt.ctfeGlobal, frame.locals.size(),
                    globals_.size());
                if (index == inductionIndex) {
                    std::int32_t parsedStep = 0;
                    if (!tryParseInductionStep(
                            *stmt.expr, inductionIndex, frame.locals.size(),
                            globals_.size(), parsedStep)) {
                        return false;
                    }
                    if (inductionUpdates != 0 && parsedStep != step) {
                        return false;
                    }
                    step = parsedStep;
                    ++inductionUpdates;
                } else if (declarations[index] >= 0) {
                    if (declarations[index] != 0) {
                        return false;
                    }
                } else if (boundCell(
                               &frame, stmt.ctfeSlot, stmt.ctfeGlobal)->isConst) {
                    return false;
                }
                modified[index] = true;
                return true;
            }
            case StmtKind::DeclStmt: {
                const std::size_t index = denseSlotIndex(
                    stmt.decl->ctfeSlot, false, frame.locals.size(),
                    globals_.size());
                modified[index] = true;
                declarations[index] = stmt.decl->isConst ? 1 : 0;
                return true;
            }
            case StmtKind::ExprStmt:
                return validateDenseLoopExpr(*stmt.expr, frame);
            case StmtKind::If:
                return validateDenseLoopExpr(*stmt.expr, frame) &&
                       validateDenseLoopBody(
                           *stmt.thenBranch, frame, modified, declarations,
                           inductionIndex, inductionUpdates, step) &&
                       (!stmt.elseBranch ||
                        validateDenseLoopBody(
                            *stmt.elseBranch, frame, modified, declarations,
                            inductionIndex, inductionUpdates, step));
            case StmtKind::While:
            case StmtKind::Break:
            case StmtKind::Continue:
            case StmtKind::Return:
                return false;
        }
        return false;
    }

    // Leaf `return <expr>` bodies that only reference parameters — safe to
    // inline into a dense loop without allocating callee locals.
    static const Expr* denseInlineReturnExpr(const Function& function) {
        if (function.returnType != ValueType::Int || !function.body ||
            function.body->kind != StmtKind::Block ||
            function.body->statements.size() != 1) {
            return nullptr;
        }
        const Stmt& stmt = *function.body->statements.front();
        if (stmt.kind != StmtKind::Return || !stmt.expr) {
            return nullptr;
        }
        return stmt.expr.get();
    }

    bool validateDenseInlinedReturn(const Expr& expr, const Function& callee) const {
        switch (expr.kind) {
            case ExprKind::Number:
                return true;
            case ExprKind::Variable:
                return !expr.ctfeGlobal && expr.ctfeSlot >= 0 &&
                       static_cast<std::size_t>(expr.ctfeSlot) < callee.params.size();
            case ExprKind::Unary:
                return (expr.ctfeOp == CtfeOp::Positive ||
                        expr.ctfeOp == CtfeOp::Negative ||
                        expr.ctfeOp == CtfeOp::LogicalNot) &&
                       validateDenseInlinedReturn(*expr.lhs, callee);
            case ExprKind::Binary:
                if (expr.ctfeOp == CtfeOp::LogicalAnd ||
                    expr.ctfeOp == CtfeOp::LogicalOr ||
                    expr.ctfeOp == CtfeOp::None) {
                    return false;
                }
                return validateDenseInlinedReturn(*expr.lhs, callee) &&
                       validateDenseInlinedReturn(*expr.rhs, callee);
            case ExprKind::Call:
                return false;
        }
        return false;
    }

    bool validateDenseLoopExpr(const Expr& expr, Frame& frame) const {
        switch (expr.kind) {
            case ExprKind::Number:
            case ExprKind::Variable:
                return true;
            case ExprKind::Unary:
                return expr.ctfeOp == CtfeOp::Positive ||
                       expr.ctfeOp == CtfeOp::Negative ||
                       expr.ctfeOp == CtfeOp::LogicalNot
                    ? validateDenseLoopExpr(*expr.lhs, frame)
                    : false;
            case ExprKind::Binary:
                if (expr.ctfeOp == CtfeOp::LogicalAnd ||
                    expr.ctfeOp == CtfeOp::LogicalOr) {
                    return validateDenseLoopExpr(*expr.lhs, frame) &&
                           validateDenseLoopExpr(*expr.rhs, frame);
                }
                return validateDenseLoopExpr(*expr.lhs, frame) &&
                       validateDenseLoopExpr(*expr.rhs, frame);
            case ExprKind::Call: {
                if (!expr.ctfeCallee || !expr.ctfeCallee->ctfeMemoizable ||
                    expr.args.size() != expr.ctfeCallee->params.size()) {
                    return false;
                }
                const Expr* ret = denseInlineReturnExpr(*expr.ctfeCallee);
                if (!ret || !validateDenseInlinedReturn(*ret, *expr.ctfeCallee)) {
                    return false;
                }
                for (const auto& arg : expr.args) {
                    if (!validateDenseLoopExpr(*arg, frame)) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

    struct DenseEmitter {
        Frame& frame;
        std::size_t globalCount;
        std::vector<std::int32_t>& code;
        bool ok = true;

        std::size_t slotIndex(int slot, bool global) const {
            return denseSlotIndex(slot, global, frame.locals.size(), globalCount);
        }

        void emit(DenseOp op) { code.push_back(op); }

        void emit(DenseOp op, std::int32_t operand) {
            code.push_back(op);
            code.push_back(operand);
        }

        std::size_t emitJumpPlaceholder(DenseOp op) {
            emit(op, 0);
            return code.size() - 1;
        }

        void patchJump(std::size_t operandIndex, std::size_t target) {
            code[operandIndex] = static_cast<std::int32_t>(target);
        }

        void compileInlinedReturn(const Expr& expr, const Function& callee,
                                  const Expr& call) {
            switch (expr.kind) {
                case ExprKind::Number:
                    emit(DnLoadImm, toInt32(expr.number));
                    return;
                case ExprKind::Variable: {
                    if (expr.ctfeGlobal || expr.ctfeSlot < 0 ||
                        static_cast<std::size_t>(expr.ctfeSlot) >= callee.params.size() ||
                        static_cast<std::size_t>(expr.ctfeSlot) >= call.args.size()) {
                        ok = false;
                        return;
                    }
                    compileExpr(*call.args[static_cast<std::size_t>(expr.ctfeSlot)]);
                    return;
                }
                case ExprKind::Unary:
                    compileInlinedReturn(*expr.lhs, callee, call);
                    if (!ok) {
                        return;
                    }
                    if (expr.ctfeOp == CtfeOp::Positive) {
                        return;
                    }
                    if (expr.ctfeOp == CtfeOp::Negative) {
                        emit(DnNeg);
                        return;
                    }
                    if (expr.ctfeOp == CtfeOp::LogicalNot) {
                        emit(DnNot);
                        return;
                    }
                    ok = false;
                    return;
                case ExprKind::Binary:
                    compileInlinedReturn(*expr.lhs, callee, call);
                    if (!ok) {
                        return;
                    }
                    compileInlinedReturn(*expr.rhs, callee, call);
                    if (!ok) {
                        return;
                    }
                    switch (expr.ctfeOp) {
                        case CtfeOp::Add:
                            emit(DnAdd);
                            return;
                        case CtfeOp::Subtract:
                            emit(DnSub);
                            return;
                        case CtfeOp::Multiply:
                            emit(DnMul);
                            return;
                        case CtfeOp::Divide:
                            emit(DnDiv);
                            return;
                        case CtfeOp::Remainder:
                            emit(DnRem);
                            return;
                        case CtfeOp::Less:
                            emit(DnLt);
                            return;
                        case CtfeOp::Greater:
                            emit(DnGt);
                            return;
                        case CtfeOp::LessEqual:
                            emit(DnLe);
                            return;
                        case CtfeOp::GreaterEqual:
                            emit(DnGe);
                            return;
                        case CtfeOp::Equal:
                            emit(DnEq);
                            return;
                        case CtfeOp::NotEqual:
                            emit(DnNe);
                            return;
                        default:
                            ok = false;
                            return;
                    }
                case ExprKind::Call:
                    ok = false;
                    return;
            }
            ok = false;
        }

        void compileExpr(const Expr& expr) {
            switch (expr.kind) {
                case ExprKind::Number:
                    emit(DnLoadImm, toInt32(expr.number));
                    return;
                case ExprKind::Variable:
                    emit(DnLoadSlot,
                         static_cast<std::int32_t>(
                             slotIndex(expr.ctfeSlot, expr.ctfeGlobal)));
                    return;
                case ExprKind::Unary:
                    compileExpr(*expr.lhs);
                    if (expr.ctfeOp == CtfeOp::Positive) {
                        return;
                    }
                    if (expr.ctfeOp == CtfeOp::Negative) {
                        emit(DnNeg);
                        return;
                    }
                    if (expr.ctfeOp == CtfeOp::LogicalNot) {
                        emit(DnNot);
                        return;
                    }
                    ok = false;
                    return;
                case ExprKind::Binary:
                    if (expr.ctfeOp == CtfeOp::LogicalAnd) {
                        compileExpr(*expr.lhs);
                        const std::size_t falseJump = emitJumpPlaceholder(DnJumpIf0);
                        compileExpr(*expr.rhs);
                        emit(DnNot);
                        emit(DnNot);
                        const std::size_t endJump = emitJumpPlaceholder(DnJump);
                        patchJump(falseJump, code.size());
                        emit(DnLoadImm, 0);
                        patchJump(endJump, code.size());
                        return;
                    }
                    if (expr.ctfeOp == CtfeOp::LogicalOr) {
                        compileExpr(*expr.lhs);
                        const std::size_t trueJump = emitJumpPlaceholder(DnJumpIfNot0);
                        compileExpr(*expr.rhs);
                        emit(DnNot);
                        emit(DnNot);
                        const std::size_t endJump = emitJumpPlaceholder(DnJump);
                        patchJump(trueJump, code.size());
                        emit(DnLoadImm, 1);
                        patchJump(endJump, code.size());
                        return;
                    }
                    compileExpr(*expr.lhs);
                    compileExpr(*expr.rhs);
                    switch (expr.ctfeOp) {
                        case CtfeOp::Add:
                            emit(DnAdd);
                            return;
                        case CtfeOp::Subtract:
                            emit(DnSub);
                            return;
                        case CtfeOp::Multiply:
                            emit(DnMul);
                            return;
                        case CtfeOp::Divide:
                            emit(DnDiv);
                            return;
                        case CtfeOp::Remainder:
                            emit(DnRem);
                            return;
                        case CtfeOp::Less:
                            emit(DnLt);
                            return;
                        case CtfeOp::Greater:
                            emit(DnGt);
                            return;
                        case CtfeOp::LessEqual:
                            emit(DnLe);
                            return;
                        case CtfeOp::GreaterEqual:
                            emit(DnGe);
                            return;
                        case CtfeOp::Equal:
                            emit(DnEq);
                            return;
                        case CtfeOp::NotEqual:
                            emit(DnNe);
                            return;
                        default:
                            ok = false;
                            return;
                    }
                case ExprKind::Call: {
                    if (!expr.ctfeCallee) {
                        ok = false;
                        return;
                    }
                    const Expr* ret = denseInlineReturnExpr(*expr.ctfeCallee);
                    if (!ret) {
                        ok = false;
                        return;
                    }
                    compileInlinedReturn(*ret, *expr.ctfeCallee, expr);
                    return;
                }
            }
            ok = false;
        }

        void compileStmt(const Stmt& stmt) {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        compileStmt(*child);
                    }
                    return;
                case StmtKind::Empty:
                    return;
                case StmtKind::ExprStmt:
                    compileExpr(*stmt.expr);
                    emit(DnPop);
                    return;
                case StmtKind::Assign:
                    compileExpr(*stmt.expr);
                    emit(DnStoreSlot,
                          static_cast<std::int32_t>(
                              slotIndex(stmt.ctfeSlot, stmt.ctfeGlobal)));
                    return;
                case StmtKind::DeclStmt:
                    compileExpr(*stmt.decl->init);
                    emit(DnStoreSlot,
                          static_cast<std::int32_t>(
                              slotIndex(stmt.decl->ctfeSlot, false)));
                    return;
                case StmtKind::If: {
                    compileExpr(*stmt.expr);
                    const std::size_t elseJump = emitJumpPlaceholder(DnJumpIf0);
                    compileStmt(*stmt.thenBranch);
                    if (!stmt.elseBranch) {
                        patchJump(elseJump, code.size());
                        return;
                    }
                    const std::size_t endJump = emitJumpPlaceholder(DnJump);
                    patchJump(elseJump, code.size());
                    compileStmt(*stmt.elseBranch);
                    patchJump(endJump, code.size());
                    return;
                }
                default:
                    ok = false;
                    return;
            }
        }
    };

    static void runDenseProgram(const std::vector<std::int32_t>& code,
                                std::vector<std::int32_t>& slots,
                                std::vector<std::int32_t>& stack) {
        stack.clear();
        std::size_t ip = 0;
        while (ip < code.size()) {
            const DenseOp op = static_cast<DenseOp>(code[ip++]);
            switch (op) {
                case DnLoadSlot:
                    stack.push_back(slots[static_cast<std::size_t>(code[ip++])]);
                    break;
                case DnLoadImm:
                    stack.push_back(code[ip++]);
                    break;
                case DnStoreSlot: {
                    const auto index = static_cast<std::size_t>(code[ip++]);
                    slots[index] = popStack(stack);
                    break;
                }
                case DnAdd: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) + rhs));
                    break;
                }
                case DnSub: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) - rhs));
                    break;
                }
                case DnMul: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) * rhs));
                    break;
                }
                case DnDiv: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    if (rhs == 0) {
                        throw CtfeAbort{};
                    }
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) / rhs));
                    break;
                }
                case DnRem: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    if (rhs == 0) {
                        throw CtfeAbort{};
                    }
                    stack.push_back(toInt32(static_cast<std::int64_t>(lhs) % rhs));
                    break;
                }
                case DnNeg:
                    stack.push_back(
                        toInt32(-static_cast<std::int64_t>(popStack(stack))));
                    break;
                case DnNot:
                    stack.push_back(popStack(stack) == 0 ? 1 : 0);
                    break;
                case DnLt: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs < rhs ? 1 : 0);
                    break;
                }
                case DnGt: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs > rhs ? 1 : 0);
                    break;
                }
                case DnLe: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs <= rhs ? 1 : 0);
                    break;
                }
                case DnGe: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs >= rhs ? 1 : 0);
                    break;
                }
                case DnEq: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs == rhs ? 1 : 0);
                    break;
                }
                case DnNe: {
                    const std::int32_t rhs = popStack(stack);
                    const std::int32_t lhs = popStack(stack);
                    stack.push_back(lhs != rhs ? 1 : 0);
                    break;
                }
                case DnPop:
                    popStack(stack);
                    break;
                case DnJump:
                    ip = static_cast<std::size_t>(code[ip]);
                    break;
                case DnJumpIf0: {
                    const std::size_t target = static_cast<std::size_t>(code[ip++]);
                    if (popStack(stack) == 0) {
                        ip = target;
                    }
                    break;
                }
                case DnJumpIfNot0: {
                    const std::size_t target = static_cast<std::size_t>(code[ip++]);
                    if (popStack(stack) != 0) {
                        ip = target;
                    }
                    break;
                }
            }
        }
    }

    bool tryExecuteNativeDenseLoop(const Stmt& stmt, Frame& frame) {
        const Expr& condition = *stmt.expr;
        if (condition.kind != ExprKind::Binary) {
            return false;
        }
        const CtfeOp rawOp = condition.ctfeOp;
        if (rawOp != CtfeOp::Less && rawOp != CtfeOp::LessEqual &&
            rawOp != CtfeOp::Greater && rawOp != CtfeOp::GreaterEqual) {
            return false;
        }

        const auto isNumberOrVar = [](const Expr& expr) {
            return expr.kind == ExprKind::Number || expr.kind == ExprKind::Variable;
        };

        const Expr* inductionExpr = nullptr;
        const Expr* boundExpr = nullptr;
        CtfeOp op = rawOp;
        if (condition.lhs->kind == ExprKind::Variable && isNumberOrVar(*condition.rhs)) {
            inductionExpr = condition.lhs.get();
            boundExpr = condition.rhs.get();
        } else if (condition.rhs->kind == ExprKind::Variable &&
                   isNumberOrVar(*condition.lhs)) {
            inductionExpr = condition.rhs.get();
            boundExpr = condition.lhs.get();
            op = flipComparison(rawOp);
        } else {
            return false;
        }

        const std::size_t stateCount = frame.locals.size() + globals_.size();
        if (stateCount + 1 > kAffineDimensionLimit) {
            return false;
        }

        const std::size_t inductionIndex = denseSlotIndex(
            inductionExpr->ctfeSlot, inductionExpr->ctfeGlobal,
            frame.locals.size(), globals_.size());

        std::vector<bool> modified(stateCount, false);
        std::vector<std::int8_t> declarations(stateCount, -1);
        int inductionUpdates = 0;
        std::int32_t step = 0;
        if (!validateDenseLoopBody(
                *stmt.thenBranch, frame, modified, declarations,
                inductionIndex, inductionUpdates, step)) {
            return false;
        }
        if (inductionUpdates != 1) {
            return false;
        }

        Cell* inductionCell = boundCell(
            &frame, inductionExpr->ctfeSlot, inductionExpr->ctfeGlobal);

        std::int32_t bound = 0;
        if (boundExpr->kind == ExprKind::Number) {
            bound = toInt32(boundExpr->number);
        } else {
            const std::size_t boundIndex = denseSlotIndex(
                boundExpr->ctfeSlot, boundExpr->ctfeGlobal,
                frame.locals.size(), globals_.size());
            if (modified[boundIndex]) {
                return false;
            }
            bound = boundCell(
                &frame, boundExpr->ctfeSlot, boundExpr->ctfeGlobal)->value;
        }

        const std::int32_t start = inductionCell->value;
        const bool increasing = op == CtfeOp::Less || op == CtfeOp::LessEqual;
        if (increasing) {
            if (step <= 0) {
                return false;
            }
            if ((op == CtfeOp::Less && start >= bound) ||
                (op == CtfeOp::LessEqual && start > bound)) {
                return false;
            }
        } else if (step >= 0) {
            return false;
        } else if ((op == CtfeOp::Greater && start <= bound) ||
                   (op == CtfeOp::GreaterEqual && start < bound)) {
            return false;
        }

        std::uint64_t iterations = 0;
        if (increasing) {
            const std::int64_t distance =
                static_cast<std::int64_t>(bound) - start;
            if (op == CtfeOp::Less) {
                iterations = static_cast<std::uint64_t>(
                    (distance + step - 1) / step);
            } else {
                iterations = static_cast<std::uint64_t>(distance / step + 1);
            }
        } else {
            const std::int64_t distance =
                static_cast<std::int64_t>(start) - bound;
            const std::int64_t negStep = -static_cast<std::int64_t>(step);
            if (op == CtfeOp::Greater) {
                iterations = static_cast<std::uint64_t>(
                    (distance + negStep - 1) / negStep);
            } else {
                iterations = static_cast<std::uint64_t>(distance / negStep + 1);
            }
        }
        if (iterations < kAffineMinIterations) {
            return false;
        }

        const std::int64_t finalInduction =
            static_cast<std::int64_t>(start) +
            static_cast<std::int64_t>(iterations) * step;
        if (finalInduction > std::numeric_limits<std::int32_t>::max() ||
            finalInduction < std::numeric_limits<std::int32_t>::min()) {
            return false;
        }
        if ((op == CtfeOp::Less && finalInduction < bound) ||
            (op == CtfeOp::LessEqual && finalInduction <= bound) ||
            (op == CtfeOp::Greater && finalInduction > bound) ||
            (op == CtfeOp::GreaterEqual && finalInduction >= bound)) {
            return false;
        }

        std::vector<std::int32_t> program;
        program.reserve(64);
        DenseEmitter emitter{frame, globals_.size(), program};
        emitter.compileStmt(*stmt.thenBranch);
        if (!emitter.ok || program.empty()) {
            return false;
        }

        std::vector<std::int32_t> slots(stateCount, 0);
        for (std::size_t i = 0; i < frame.locals.size(); ++i) {
            if (frame.locals[i].initialized) {
                slots[i] = frame.locals[i].value;
            }
        }
        for (std::size_t i = 0; i < globals_.size(); ++i) {
            slots[frame.locals.size() + i] = globals_[i].value;
        }

        std::vector<std::int32_t> stack;
        stack.reserve(32);
        FastLoopGuard fast(fastLoopDepth_);
        std::uint64_t done = 0;
        while (done < iterations) {
            if ((done & (kLoopTickBatch - 1)) == 0) {
                tickMany(kLoopTickBatch);
            }
            runDenseProgram(program, slots, stack);
            ++done;
        }

        for (std::size_t i = 0; i < frame.locals.size(); ++i) {
            if (frame.locals[i].initialized || modified[i] || declarations[i] >= 0) {
                frame.locals[i].value = slots[i];
            }
            if (declarations[i] >= 0) {
                frame.locals[i].isConst = declarations[i] != 0;
                frame.locals[i].initialized = true;
            }
        }
        for (std::size_t i = 0; i < globals_.size(); ++i) {
            if (modified[frame.locals.size() + i]) {
                globals_[i].value = slots[frame.locals.size() + i];
            }
        }
        return true;
    }

    bool tryExecuteAffineLoop(const Stmt& stmt, Frame& frame) {
        const Expr& condition = *stmt.expr;
        if (condition.kind != ExprKind::Binary) {
            return false;
        }
        const CtfeOp rawOp = condition.ctfeOp;
        if (rawOp != CtfeOp::Less && rawOp != CtfeOp::LessEqual &&
            rawOp != CtfeOp::Greater && rawOp != CtfeOp::GreaterEqual) {
            return false;
        }

        const auto isNumberOrVar = [](const Expr& expr) {
            return expr.kind == ExprKind::Number || expr.kind == ExprKind::Variable;
        };

        // Normalize so the induction variable is on the left:
        // `n > i` becomes `i < n`, etc.
        const Expr* inductionExpr = nullptr;
        const Expr* boundExpr = nullptr;
        CtfeOp op = rawOp;
        if (condition.lhs->kind == ExprKind::Variable && isNumberOrVar(*condition.rhs)) {
            inductionExpr = condition.lhs.get();
            boundExpr = condition.rhs.get();
        } else if (condition.rhs->kind == ExprKind::Variable &&
                   isNumberOrVar(*condition.lhs)) {
            inductionExpr = condition.rhs.get();
            boundExpr = condition.lhs.get();
            op = flipComparison(rawOp);
        } else {
            return false;
        }

        const std::size_t stateCount = frame.locals.size() + globals_.size();
        const std::size_t dimension = stateCount + 1;
        if (dimension > kAffineDimensionLimit) {
            return false;
        }

        std::vector<bool> modified(stateCount, false);
        std::vector<std::int8_t> declarations(stateCount, -1);
        if (!collectAffineAssignments(
                *stmt.thenBranch, frame, modified, declarations)) {
            return false;
        }

        Cell* inductionCell = boundCell(
            &frame, inductionExpr->ctfeSlot, inductionExpr->ctfeGlobal);
        const std::size_t inductionIndex = affineStateIndex(
            inductionExpr->ctfeSlot, inductionExpr->ctfeGlobal,
            frame.locals.size());
        if (!modified[inductionIndex]) {
            return false;
        }

        std::int32_t bound = 0;
        if (boundExpr->kind == ExprKind::Number) {
            bound = toInt32(boundExpr->number);
        } else {
            Cell* boundValue = boundCell(
                &frame, boundExpr->ctfeSlot, boundExpr->ctfeGlobal);
            const std::size_t boundIndex = affineStateIndex(
                boundExpr->ctfeSlot, boundExpr->ctfeGlobal,
                frame.locals.size());
            if (modified[boundIndex]) {
                return false;
            }
            bound = boundValue->value;
        }

        AffineMatrix transform(dimension, AffineVector(dimension, 0));
        for (std::size_t i = 0; i < dimension; ++i) {
            transform[i][i] = 1;
        }
        if (!buildAffineTransform(
                *stmt.thenBranch, frame, transform, modified)) {
            return false;
        }

        const AffineVector& induction = transform[inductionIndex];
        for (std::size_t i = 0; i < stateCount; ++i) {
            const std::uint32_t expected = i == inductionIndex ? 1u : 0u;
            if (induction[i] != expected) {
                return false;
            }
        }
        const std::int32_t step = static_cast<std::int32_t>(induction.back());
        const std::int32_t start = inductionCell->value;
        const bool increasing = op == CtfeOp::Less || op == CtfeOp::LessEqual;
        if (increasing) {
            if (step <= 0) {
                return false;
            }
            if ((op == CtfeOp::Less && start >= bound) ||
                (op == CtfeOp::LessEqual && start > bound)) {
                return false;
            }
        } else if (step >= 0) {
            return false;
        } else if ((op == CtfeOp::Greater && start <= bound) ||
                   (op == CtfeOp::GreaterEqual && start < bound)) {
            return false;
        }

        std::uint64_t iterations = 0;
        if (increasing) {
            const std::int64_t distance =
                static_cast<std::int64_t>(bound) - start;
            if (op == CtfeOp::Less) {
                iterations = static_cast<std::uint64_t>(
                    (distance + step - 1) / step);
            } else {
                iterations = static_cast<std::uint64_t>(distance / step + 1);
            }
        } else {
            const std::int64_t distance =
                static_cast<std::int64_t>(start) - bound;
            const std::int64_t negStep = -static_cast<std::int64_t>(step);
            if (op == CtfeOp::Greater) {
                iterations = static_cast<std::uint64_t>(
                    (distance + negStep - 1) / negStep);
            } else {
                iterations = static_cast<std::uint64_t>(distance / negStep + 1);
            }
        }
        if (iterations < kAffineMinIterations) {
            return false;
        }

        const std::int64_t finalInduction =
            static_cast<std::int64_t>(start) +
            static_cast<std::int64_t>(iterations) * step;
        if (finalInduction > std::numeric_limits<std::int32_t>::max() ||
            finalInduction < std::numeric_limits<std::int32_t>::min()) {
            return false;
        }
        if ((op == CtfeOp::Less && finalInduction < bound) ||
            (op == CtfeOp::LessEqual && finalInduction <= bound) ||
            (op == CtfeOp::Greater && finalInduction > bound) ||
            (op == CtfeOp::GreaterEqual && finalInduction >= bound)) {
            return false;
        }

        AffineVector state(dimension, 0);
        for (std::size_t i = 0; i < frame.locals.size(); ++i) {
            state[i] = static_cast<std::uint32_t>(frame.locals[i].value);
        }
        for (std::size_t i = 0; i < globals_.size(); ++i) {
            state[frame.locals.size() + i] =
                static_cast<std::uint32_t>(globals_[i].value);
        }
        state.back() = 1;

        FastLoopGuard fast(fastLoopDepth_);
        AffineMatrix power = std::move(transform);
        std::uint64_t remaining = iterations;
        while (remaining != 0) {
            if ((remaining & 1u) != 0) {
                state = multiplyAffine(power, state);
            }
            remaining >>= 1;
            if (remaining != 0) {
                power = multiplyAffine(power, power);
            }
        }

        for (std::size_t i = 0; i < frame.locals.size(); ++i) {
            if (frame.locals[i].initialized || declarations[i] >= 0) {
                frame.locals[i].value = static_cast<std::int32_t>(state[i]);
            }
            if (declarations[i] >= 0) {
                frame.locals[i].isConst = declarations[i] != 0;
                frame.locals[i].initialized = true;
            }
        }
        for (std::size_t i = 0; i < globals_.size(); ++i) {
            globals_[i].value = static_cast<std::int32_t>(
                state[frame.locals.size() + i]);
        }
        return true;
    }
    bool evalSimpleCompareCondition(const Expr& expr, Frame* frame, bool& result) const {
        if (expr.kind != ExprKind::Binary) {
            return false;
        }
        std::int32_t lhs = 0;
        std::int32_t rhs = 0;
        if (expr.lhs->kind == ExprKind::Variable) {
            lhs = readSlot(frame, expr.lhs->ctfeSlot, expr.lhs->ctfeGlobal);
        } else if (expr.lhs->kind == ExprKind::Number) {
            lhs = toInt32(expr.lhs->number);
        } else {
            return false;
        }
        if (expr.rhs->kind == ExprKind::Variable) {
            rhs = readSlot(frame, expr.rhs->ctfeSlot, expr.rhs->ctfeGlobal);
        } else if (expr.rhs->kind == ExprKind::Number) {
            rhs = toInt32(expr.rhs->number);
        } else {
            return false;
        }
        switch (expr.ctfeOp) {
            case CtfeOp::Less:
                result = lhs < rhs;
                return true;
            case CtfeOp::Greater:
                result = lhs > rhs;
                return true;
            case CtfeOp::LessEqual:
                result = lhs <= rhs;
                return true;
            case CtfeOp::GreaterEqual:
                result = lhs >= rhs;
                return true;
            case CtfeOp::Equal:
                result = lhs == rhs;
                return true;
            case CtfeOp::NotEqual:
                result = lhs != rhs;
                return true;
            default:
                return false;
        }
    }

    Flow executeStmt(const Stmt& stmt, Frame& frame, bool accountSteps = true) {
        // Empty/While handle their own accounting; skip the shared pre-tick.
        if (accountSteps && stmt.kind != StmtKind::Empty && stmt.kind != StmtKind::While) {
            tick();
        }
        switch (stmt.kind) {
            case StmtKind::Block:
                for (const auto& child : stmt.statements) {
                    Flow flow = executeStmt(*child, frame, accountSteps);
                    if (flow.kind != FlowKind::Normal) {
                        return flow;
                    }
                }
                return {};
            case StmtKind::Empty:
                return {};
            case StmtKind::ExprStmt:
                evalExpr(*stmt.expr, &frame);
                return {};
            case StmtKind::Assign: {
                const std::int32_t value = evalExpr(*stmt.expr, &frame);
                writeSlot(&frame, stmt.ctfeSlot, stmt.ctfeGlobal, value);
                return {};
            }
            case StmtKind::DeclStmt: {
                const Decl& decl = *stmt.decl;
                const std::int32_t value = evalExpr(*decl.init, &frame);
                Cell* cell = boundCell(&frame, decl.ctfeSlot, false, false);
                *cell = Cell{value, decl.isConst, true};
                return {};
            }
            case StmtKind::If:
                if (evalExpr(*stmt.expr, &frame) != 0) {
                    return executeStmt(*stmt.thenBranch, frame, accountSteps);
                }
                if (stmt.elseBranch) {
                    return executeStmt(*stmt.elseBranch, frame, accountSteps);
                }
                return {};
            case StmtKind::While:
                if (tryExecuteAffineLoop(stmt, frame)) {
                    return {};
                }
                if (tryExecuteNativeDenseLoop(stmt, frame)) {
                    return {};
                }
                // Batch step accounting: one host check per 64 iterations.
                {
                    std::uint64_t iters = 0;
                    while (true) {
                        if ((iters++ & (kLoopTickBatch - 1)) == 0) {
                            tickMany(kLoopTickBatch);
                        }
                        bool keepGoing = false;
                        if (!evalSimpleCompareCondition(*stmt.expr, &frame, keepGoing)) {
                            keepGoing = evalExpr(*stmt.expr, &frame) != 0;
                        }
                        if (!keepGoing) {
                            break;
                        }
                        Flow flow = executeStmt(*stmt.thenBranch, frame, false);
                        if (flow.kind == FlowKind::Break) {
                            return {};
                        }
                        if (flow.kind == FlowKind::Continue ||
                            flow.kind == FlowKind::Normal) {
                            continue;
                        }
                        return flow;
                    }
                }
                return {};
            case StmtKind::Break:
                return Flow{FlowKind::Break, 0};
            case StmtKind::Continue:
                return Flow{FlowKind::Continue, 0};
            case StmtKind::Return:
                if (!stmt.expr) {
                    return Flow{FlowKind::Return, 0};
                }
                if (stmt.expr->kind == ExprKind::Call &&
                    stmt.expr->ctfeCallee == frame.function) {
                    frame.tailArgs.clear();
                    frame.tailArgs.reserve(stmt.expr->args.size());
                    for (const auto& arg : stmt.expr->args) {
                        frame.tailArgs.push_back(evalExpr(*arg, &frame));
                    }
                    return Flow{FlowKind::TailCall, 0};
                }
                return Flow{FlowKind::Return, evalExpr(*stmt.expr, &frame)};
        }
        throw CtfeAbort{};
    }

    std::int32_t evalExpr(const Expr& expr, Frame* frame) {
        switch (expr.kind) {
            case ExprKind::Number:
                return toInt32(expr.number);
            case ExprKind::Variable:
                return readSlot(frame, expr.ctfeSlot, expr.ctfeGlobal);
            case ExprKind::Unary: {
                const std::int32_t value = evalExpr(*expr.lhs, frame);
                switch (expr.ctfeOp) {
                    case CtfeOp::Positive:
                        return value;
                    case CtfeOp::Negative:
                        return toInt32(-static_cast<std::int64_t>(value));
                    case CtfeOp::LogicalNot:
                        return value == 0 ? 1 : 0;
                    default:
                        throw CtfeAbort{};
                }
            }
            case ExprKind::Binary:
                return evalBinary(expr, frame);
            case ExprKind::Call: {
                if (!expr.ctfeCallee) {
                    throw CtfeAbort{};
                }
                std::vector<std::int32_t> args;
                args.reserve(expr.args.size());
                for (const auto& arg : expr.args) {
                    args.push_back(evalExpr(*arg, frame));
                }
                return invoke(*expr.ctfeCallee, std::move(args));
            }
        }
        throw CtfeAbort{};
    }

    std::int32_t evalBinary(const Expr& expr, Frame* frame) {
        const std::int32_t lhs = evalExpr(*expr.lhs, frame);
        if (expr.ctfeOp == CtfeOp::LogicalAnd) {
            return lhs == 0 ? 0 : (evalExpr(*expr.rhs, frame) != 0 ? 1 : 0);
        }
        if (expr.ctfeOp == CtfeOp::LogicalOr) {
            return lhs != 0 ? 1 : (evalExpr(*expr.rhs, frame) != 0 ? 1 : 0);
        }

        const std::int32_t rhs = evalExpr(*expr.rhs, frame);
        const std::int64_t left = lhs;
        const std::int64_t right = rhs;
        switch (expr.ctfeOp) {
            case CtfeOp::Add:
                return toInt32(left + right);
            case CtfeOp::Subtract:
                return toInt32(left - right);
            case CtfeOp::Multiply:
                return toInt32(left * right);
            case CtfeOp::Divide:
                if (rhs == 0) {
                    throw CtfeAbort{};
                }
                return toInt32(left / right);
            case CtfeOp::Remainder:
                if (rhs == 0) {
                    throw CtfeAbort{};
                }
                return toInt32(left % right);
            case CtfeOp::Less:
                return lhs < rhs ? 1 : 0;
            case CtfeOp::Greater:
                return lhs > rhs ? 1 : 0;
            case CtfeOp::LessEqual:
                return lhs <= rhs ? 1 : 0;
            case CtfeOp::GreaterEqual:
                return lhs >= rhs ? 1 : 0;
            case CtfeOp::Equal:
                return lhs == rhs ? 1 : 0;
            case CtfeOp::NotEqual:
                return lhs != rhs ? 1 : 0;
            default:
                throw CtfeAbort{};
        }
    }
};


} // namespace

std::optional<std::int32_t> evaluateWholeProgram(CompUnit& unit) {
    WholeProgramEvaluator evaluator(unit);
    return evaluator.evaluate();
}

std::string emitConstantProgram(std::int32_t value) {
    std::ostringstream out;
    out << "    .text\n";
    out << "    .globl main\n";
    out << "main:\n";
    out << "    # toyc-ctfe: hit\n";
    out << "    li a0, " << printableI32(value) << '\n';
    out << "    ret\n";
    return out.str();
}


} // namespace toyc
