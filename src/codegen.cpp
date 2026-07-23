#include "codegen.hpp"

#include <chrono>
#include <functional>
#include <initializer_list>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace toyc {
namespace {

enum class SymbolKind {
    LocalVar,
    GlobalVar,
    Const,
};

struct Symbol {
    SymbolKind kind = SymbolKind::LocalVar;
    int offset = 0;
    std::string label;
    std::int32_t constValue = 0;
    std::string reg;
    int localRegIndex = -1;
    bool ownsLocalRegister = false;
};

struct FunctionInfo {
    ValueType returnType = ValueType::Int;
    std::vector<std::string> params;
    std::string label;
    const Function* function = nullptr;
    const Stmt* inlineBlock = nullptr;
    std::size_t inlineDeclCount = 0;
    const Expr* inlineReturn = nullptr;
    const Expr* inlineCondition = nullptr;
    const Expr* inlineThenReturn = nullptr;
    const Expr* inlineElseReturn = nullptr;
};

struct InlineBranchShape {
    std::size_t declCount = 0;
    const Expr* condition = nullptr;
    const Expr* thenReturn = nullptr;
    const Expr* elseReturn = nullptr;
};

std::int32_t toInt32(std::int64_t value) {
    return static_cast<std::int32_t>(value);
}

std::int64_t printableI32(std::int32_t value) {
    return static_cast<std::int64_t>(value);
}

bool fitsI12(int value) {
    return value >= -2048 && value <= 2047;
}

std::optional<int> powerOfTwoShift(std::int64_t value) {
    if (value == 0) {
        return std::nullopt;
    }
    std::uint64_t magnitude = value < 0 ? static_cast<std::uint64_t>(-value)
                                        : static_cast<std::uint64_t>(value);
    if ((magnitude & (magnitude - 1)) != 0 || magnitude > (1ull << 31)) {
        return std::nullopt;
    }
    int shift = 0;
    while (magnitude > 1) {
        magnitude >>= 1;
        ++shift;
    }
    return shift;
}

int alignTo16(int value) {
    return (value + 15) / 16 * 16;
}

std::string functionLabel(const std::string& name) {
    if (name == "main") {
        return "main";
    }
    return "fn_" + name;
}

std::string globalLabel(const std::string& name) {
    return "g_" + name;
}

const Expr* simpleInlineReturnExpr(const Function& function) {
    if (function.returnType != ValueType::Int || !function.body ||
        function.body->kind != StmtKind::Block || function.body->statements.empty() ||
        function.body->statements.size() > 8) {
        return nullptr;
    }
    for (std::size_t i = 0; i + 1 < function.body->statements.size(); ++i) {
        if (function.body->statements[i]->kind != StmtKind::DeclStmt) {
            return nullptr;
        }
    }
    const Stmt& stmt = *function.body->statements.back();
    if (stmt.kind != StmtKind::Return || !stmt.expr) {
        return nullptr;
    }
    return stmt.expr.get();
}

const Expr* singleReturnExpr(const Stmt& stmt) {
    const Stmt* candidate = &stmt;
    if (candidate->kind == StmtKind::Block) {
        if (candidate->statements.size() != 1) {
            return nullptr;
        }
        candidate = candidate->statements.front().get();
    }
    if (candidate->kind != StmtKind::Return || !candidate->expr) {
        return nullptr;
    }
    return candidate->expr.get();
}

std::optional<InlineBranchShape> simpleInlineBranch(const Function& function) {
    if (function.returnType != ValueType::Int || !function.body ||
        function.body->kind != StmtKind::Block || function.body->statements.empty() ||
        function.body->statements.size() > 8) {
        return std::nullopt;
    }

    std::size_t index = 0;
    while (index < function.body->statements.size() &&
           function.body->statements[index]->kind == StmtKind::DeclStmt) {
        ++index;
    }
    if (index >= function.body->statements.size()) {
        return std::nullopt;
    }

    const Stmt& branch = *function.body->statements[index];
    if (branch.kind != StmtKind::If || !branch.expr || !branch.thenBranch) {
        return std::nullopt;
    }
    const Expr* thenReturn = singleReturnExpr(*branch.thenBranch);
    if (!thenReturn) {
        return std::nullopt;
    }

    const Expr* elseReturn = nullptr;
    if (branch.elseBranch) {
        if (index + 1 != function.body->statements.size()) {
            return std::nullopt;
        }
        elseReturn = singleReturnExpr(*branch.elseBranch);
    } else {
        if (index + 2 != function.body->statements.size()) {
            return std::nullopt;
        }
        elseReturn = singleReturnExpr(*function.body->statements[index + 1]);
    }
    if (!elseReturn) {
        return std::nullopt;
    }
    return InlineBranchShape{index, branch.expr.get(), thenReturn, elseReturn};
}

void emitRegAdd(std::ostream& out, const std::string& dst, const std::string& base, int amount) {
    if (amount == 0) {
        if (dst != base) {
            out << "    addi " << dst << ", " << base << ", 0\n";
        }
        return;
    }
    if (fitsI12(amount)) {
        out << "    addi " << dst << ", " << base << ", " << amount << '\n';
    } else {
        out << "    li t6, " << amount << '\n';
        out << "    add " << dst << ", " << base << ", t6\n";
    }
}

void emitLoadOffset(std::ostream& out, const std::string& dst, int offset, const std::string& base) {
    if (fitsI12(offset)) {
        out << "    lw " << dst << ", " << offset << '(' << base << ")\n";
    } else {
        out << "    li t6, " << offset << '\n';
        out << "    add t6, " << base << ", t6\n";
        out << "    lw " << dst << ", 0(t6)\n";
    }
}

void emitStoreOffset(std::ostream& out, const std::string& src, int offset, const std::string& base) {
    if (fitsI12(offset)) {
        out << "    sw " << src << ", " << offset << '(' << base << ")\n";
    } else {
        out << "    li t6, " << offset << '\n';
        out << "    add t6, " << base << ", t6\n";
        out << "    sw " << src << ", 0(t6)\n";
    }
}


class CodeGenerator {
public:
    CodeGenerator(const CompUnit& unit, Options options) : unit_(unit), options_(options) {}

    std::string generate() {
        collectFunctions();
        collectGlobals();

        std::ostringstream out;
        if (!dataLines_.empty()) {
            out << "    .data\n";
            out << "    .align 2\n";
            for (const std::string& line : dataLines_) {
                out << line;
            }
            out << '\n';
        }

        out << "    .text\n";
        out << "    .globl main\n";
        for (const auto& function : unit_.functions) {
            out << generateFunction(*function);
        }
        return out.str();
    }

private:
    const CompUnit& unit_;
    Options options_;
    std::unordered_map<std::string, Symbol> globals_;
    std::unordered_map<std::string, FunctionInfo> functions_;
    std::vector<std::string> dataLines_;

    class FunctionEmitter {
    public:
        FunctionEmitter(CodeGenerator& parent, const Function& function)
            : parent_(parent),
              function_(function),
              bodyEntryLabel_(".L_entry_" + functionLabel(function.name)),
              returnLabel_(".L_return_" + functionLabel(function.name)) {}

        std::string emit() {
            if (parent_.options_.optimize) {
                for (const std::string& param : function_.params) {
                    localCandidateNames_.insert(param);
                }
                collectVariableUses(*function_.body, 1);
            }
            containsCall_ = parent_.options_.optimize ? stmtContainsRuntimeCall(*function_.body)
                                                      : stmtContainsCall(*function_.body);
            if (parent_.options_.optimize) {
                selectPreferredRegisterNames();
                peClear();
            }
            body_ << "    # parameters\n";
            pushScope();
            for (std::size_t i = 0; i < function_.params.size(); ++i) {
                Symbol symbol = allocateLocal(function_.params[i]);
                paramSymbols_.push_back(symbol);
                currentScope()[function_.params[i]] = symbol;
            }
            for (std::size_t index = function_.params.size(); index > 0; --index) {
                const std::size_t i = index - 1;
                const Symbol& symbol = paramSymbols_[i];
                if (i < 8) {
                    storeSymbol(symbol, argRegisterName(i));
                } else {
                    emitLoadOffset(body_, "t0", static_cast<int>((i - 8) * 4), "s0");
                    storeSymbol(symbol, "t0");
                }
            }

            body_ << bodyEntryLabel_ << ":\n";
            const bool hasFinalReturn = emitFunctionBody();
            const bool omitFrame = canOmitFrame();
            if (!hasFinalReturn) {
                body_ << "    li a0, 0\n";
            }
            body_ << returnLabel_ << ":\n";
            if (omitFrame) {
                body_ << "    ret\n\n";
            } else {
                restoreSavedLocalRegisters();
                emitLoadOffset(body_, "ra", -4, "s0");
                emitLoadOffset(body_, "t0", -8, "s0");
                body_ << "    addi sp, s0, 0\n";
                body_ << "    addi s0, t0, 0\n";
                body_ << "    ret\n\n";
            }

            const int frameSize = alignTo16(frameHeaderSize() + slotCount_ * 4);
            std::ostringstream out;
            out << functionLabel(function_.name) << ":\n";
            if (!omitFrame) {
                emitRegAdd(out, "sp", "sp", -frameSize);
                emitStoreOffset(out, "ra", frameSize - 4, "sp");
                emitStoreOffset(out, "s0", frameSize - 8, "sp");
                emitRegAdd(out, "s0", "sp", frameSize);
                saveLocalRegisters(out);
            }
            out << body_.str();
            return out.str();
        }

    private:
        struct CachedLoopExpr {
            const Expr* expr = nullptr;
            std::string reg;
            std::string inductionName;
            const Expr* stride = nullptr;
        };

        struct InductionMultiplyCandidate {
            const Expr* product = nullptr;
            std::string inductionName;
            const Expr* stride = nullptr;
        };

        CodeGenerator& parent_;
        const Function& function_;
        std::ostringstream body_;
        std::vector<std::unordered_map<std::string, Symbol>> scopes_;
        std::vector<std::pair<std::string, std::string>> loopStack_;
        std::vector<CachedLoopExpr> cachedLoopExprs_;
        int slotCount_ = 0;
        int localRegCount_ = 0;
        int tempRegDepth_ = 0;
        int tempStackDepth_ = 0;
        std::vector<bool> tempLocationIsReg_;
        int labelCounter_ = 0;
        int inlineDepth_ = 0;
        bool containsCall_ = false;
        std::vector<Symbol> paramSymbols_;
        std::unordered_set<std::string> assignedNames_;
        std::unordered_set<std::string> referencedNames_;
        std::unordered_map<std::string, std::uint64_t> variableUseWeights_;
        std::unordered_set<std::string> localCandidateNames_;
        std::unordered_set<std::string> preferredRegisterNames_;
        std::unordered_set<std::string> pendingPreferredRegisterNames_;
        std::vector<int> freeLocalRegisters_;
        std::vector<bool> localRegisterActive_;
        int activeLocalRegCount_ = 0;
        std::string bodyEntryLabel_;
        std::string returnLabel_;

        int peExecDepth_ = 0;
        mutable int peCallDepth_ = 0;
        std::chrono::steady_clock::time_point peDeadline_{};

        struct PeValue {
            bool known = false;
            std::int32_t value = 0;
        };

        // Keep PE short: platform compile timeouts dominate score more than
        // missing a fold. Prefer skip over burning the compile budget.
        static constexpr std::uint64_t kPeLoopBudget = 5'000'000;
        static constexpr int kPeTimeLimitMilliseconds = 800;

        std::vector<std::unordered_map<std::string, PeValue>> mutable peScopes_;

        void peClear() {
            peScopes_.clear();
            peScopes_.push_back({});
        }

        void pePushScope() const {
            peScopes_.push_back({});
        }

        void pePopScope() const {
            if (peScopes_.size() > 1) {
                peScopes_.pop_back();
            }
        }

        std::optional<std::int32_t> peLookup(const std::string& name) const {
            for (auto it = peScopes_.rbegin(); it != peScopes_.rend(); ++it) {
                const auto found = it->find(name);
                if (found != it->end()) {
                    if (!found->second.known) {
                        return std::nullopt;
                    }
                    return found->second.value;
                }
            }
            return std::nullopt;
        }

        void peSetKnown(const std::string& name, std::int32_t value) {
            for (auto it = peScopes_.rbegin(); it != peScopes_.rend(); ++it) {
                const auto found = it->find(name);
                if (found != it->end()) {
                    found->second = PeValue{true, value};
                    return;
                }
            }
            peScopes_.back()[name] = PeValue{true, value};
        }

        void peInvalidate(const std::string& name) {
            for (auto it = peScopes_.rbegin(); it != peScopes_.rend(); ++it) {
                const auto found = it->find(name);
                if (found != it->end()) {
                    found->second.known = false;
                    return;
                }
            }
            peScopes_.back()[name] = PeValue{false, 0};
        }

        void peDeclare(const std::string& name, std::int32_t value) const {
            peScopes_.back()[name] = PeValue{true, value};
        }

        void peDeclareUnknown(const std::string& name) {
            peScopes_.back()[name] = PeValue{false, 0};
        }

        bool peTrackingEnabled() const {
            return peExecDepth_ > 0 || loopStack_.empty();
        }

        void bindPeLocal(const std::string& name, const Expr& init) {
            if (!parent_.options_.optimize || !peTrackingEnabled()) {
                return;
            }
            if (const auto value = evalConst(init)) {
                peSetKnown(name, *value);
            } else {
                peInvalidate(name);
            }
        }

        std::unordered_map<std::string, Symbol>& currentScope() {
            if (scopes_.empty()) {
                throw std::runtime_error("internal error: missing local scope");
            }
            return scopes_.back();
        }

        void pushScope() {
            scopes_.push_back({});
        }

        void popScope() {
            for (auto& [name, symbol] : scopes_.back()) {
                (void)name;
                releaseLocalRegister(symbol);
            }
            scopes_.pop_back();
        }

        bool canOmitFrame() const {
            return parent_.options_.optimize && !containsCall_ && function_.params.size() <= 8 &&
                   slotCount_ == 0 && savedLocalRegisterCount() == 0;
        }

        bool useRegisterLocals() const {
            return parent_.options_.optimize;
        }

        int maxSavedLocalRegisterCount() const {
            return useRegisterLocals() ? 11 : 0;
        }

        int savedLocalRegisterCount() const {
            if (!useRegisterLocals()) {
                return 0;
            }
            if (containsCall_) {
                return std::min(localRegCount_, 11);
            }
            if (localRegCount_ <= 7) {
                return 0;
            }
            return std::min(localRegCount_ - 7, 11);
        }

        int allocatableLocalRegisterCount() const {
            if (!useRegisterLocals()) {
                return 0;
            }
            return containsCall_ ? 11 : 18;
        }

        int frameHeaderSize() const {
            return 8 + maxSavedLocalRegisterCount() * 4;
        }

        std::string localRegisterName(int index) const {
            if (!containsCall_) {
                if (index < 7) {
                    return "a" + std::to_string(index + 1);
                }
                return "s" + std::to_string(index - 6);
            }
            return "s" + std::to_string(index + 1);
        }

        std::string savedLocalRegisterName(int index) const {
            return "s" + std::to_string(index + 1);
        }

        std::string argRegisterName(std::size_t index) const {
            return "a" + std::to_string(index);
        }

        void selectPreferredRegisterNames() {
            std::vector<std::pair<std::uint64_t, std::string>> ranked;
            ranked.reserve(localCandidateNames_.size());
            for (const std::string& name : localCandidateNames_) {
                const auto found = variableUseWeights_.find(name);
                if (found != variableUseWeights_.end() && found->second != 0) {
                    ranked.emplace_back(found->second, name);
                }
            }
            std::sort(ranked.begin(), ranked.end(), [](const auto& lhs, const auto& rhs) {
                if (lhs.first != rhs.first) {
                    return lhs.first > rhs.first;
                }
                return lhs.second < rhs.second;
            });
            const std::size_t limit = std::min<std::size_t>(
                ranked.size(), static_cast<std::size_t>(allocatableLocalRegisterCount()));
            for (std::size_t i = 0; i < limit; ++i) {
                preferredRegisterNames_.insert(ranked[i].second);
                pendingPreferredRegisterNames_.insert(ranked[i].second);
            }
        }

        void releaseLocalRegister(Symbol& symbol) {
            if (!symbol.ownsLocalRegister) {
                return;
            }
            const int index = symbol.localRegIndex;
            if (index < 0 || static_cast<std::size_t>(index) >= localRegisterActive_.size() ||
                !localRegisterActive_[static_cast<std::size_t>(index)]) {
                throw std::runtime_error("internal error: invalid local register release");
            }
            localRegisterActive_[static_cast<std::size_t>(index)] = false;
            freeLocalRegisters_.push_back(index);
            --activeLocalRegCount_;
            symbol.ownsLocalRegister = false;
        }

        Symbol allocateLocal(const std::string& name = {}) {
            if (useRegisterLocals()) {
                const bool named = !name.empty();
                const bool preferred =
                    !named || preferredRegisterNames_.find(name) != preferredRegisterNames_.end();
                if (named && preferred) {
                    pendingPreferredRegisterNames_.erase(name);
                }
                const int available =
                    allocatableLocalRegisterCount() - activeLocalRegCount_;
                const bool reserveForPreferred =
                    !named && available <=
                                  static_cast<int>(pendingPreferredRegisterNames_.size());
                if (preferred && available > 0 && !reserveForPreferred) {
                    int index = -1;
                    if (!freeLocalRegisters_.empty()) {
                        index = freeLocalRegisters_.back();
                        freeLocalRegisters_.pop_back();
                        localRegisterActive_[static_cast<std::size_t>(index)] = true;
                    } else {
                        index = localRegCount_++;
                        localRegisterActive_.push_back(true);
                    }
                    ++activeLocalRegCount_;
                    Symbol symbol;
                    symbol.kind = SymbolKind::LocalVar;
                    symbol.reg = localRegisterName(index);
                    symbol.localRegIndex = index;
                    symbol.ownsLocalRegister = true;
                    return symbol;
                }
            }

            Symbol symbol;
            symbol.kind = SymbolKind::LocalVar;
            symbol.offset = allocateSlot();
            return symbol;
        }

        int allocateSlot() {
            const int offset = -(frameHeaderSize() + 4 + slotCount_ * 4);
            ++slotCount_;
            return offset;
        }

        void saveLocalRegisters(std::ostream& out) const {
            if (!useRegisterLocals()) {
                return;
            }
            const int savedCount = std::min(localRegCount_, savedLocalRegisterCount());
            for (int i = 0; i < savedCount; ++i) {
                emitStoreOffset(out, savedLocalRegisterName(i), -12 - i * 4, "s0");
            }
        }

        void restoreSavedLocalRegisters() {
            if (!useRegisterLocals()) {
                return;
            }
            const int savedCount = std::min(localRegCount_, savedLocalRegisterCount());
            for (int i = 0; i < savedCount; ++i) {
                emitLoadOffset(body_, savedLocalRegisterName(i), -12 - i * 4, "s0");
            }
        }

        std::string newLabel(const std::string& hint) {
            return ".L_" + functionLabel(function_.name) + "_" + hint + "_" + std::to_string(labelCounter_++);
        }

        bool emitFunctionBody() {
            if (function_.body && function_.body->kind == StmtKind::Block &&
                !function_.body->statements.empty() &&
                function_.body->statements.back()->kind == StmtKind::Return) {
                pushScope();
                if (parent_.options_.optimize) {
                    pePushScope();
                }
                for (std::size_t i = 0; i + 1 < function_.body->statements.size(); ++i) {
                    emitStmt(*function_.body->statements[i]);
                    if (stmtAlwaysTerminates(*function_.body->statements[i])) {
                        if (parent_.options_.optimize) {
                            pePopScope();
                        }
                        popScope();
                        return true;
                    }
                }
                emitReturn(*function_.body->statements.back(), false);
                if (parent_.options_.optimize) {
                    pePopScope();
                }
                popScope();
                return true;
            }

            emitStmt(*function_.body);
            return false;
        }

        std::optional<Symbol> lookup(const std::string& name) const {
            for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
                const auto found = it->find(name);
                if (found != it->end()) {
                    return found->second;
                }
            }
            const auto global = parent_.globals_.find(name);
            if (global != parent_.globals_.end()) {
                return global->second;
            }
            return std::nullopt;
        }

        void loadSymbol(const Symbol& symbol, const std::string& dst) {
            if (symbol.kind == SymbolKind::Const) {
                body_ << "    li " << dst << ", " << printableI32(symbol.constValue) << '\n';
            } else if (symbol.kind == SymbolKind::LocalVar) {
                if (!symbol.reg.empty()) {
                    body_ << "    addi " << dst << ", " << symbol.reg << ", 0\n";
                } else {
                    emitLoadOffset(body_, dst, symbol.offset, "s0");
                }
            } else {
                body_ << "    la t0, " << symbol.label << '\n';
                body_ << "    lw " << dst << ", 0(t0)\n";
            }
        }

        void storeSymbol(const Symbol& symbol, const std::string& src) {
            if (symbol.kind == SymbolKind::LocalVar) {
                if (!symbol.reg.empty()) {
                    if (symbol.reg != src) {
                        body_ << "    addi " << symbol.reg << ", " << src << ", 0\n";
                    }
                } else {
                    emitStoreOffset(body_, src, symbol.offset, "s0");
                }
            } else {
                body_ << "    la t0, " << symbol.label << '\n';
                body_ << "    sw " << src << ", 0(t0)\n";
            }
        }

        void copySymbol(const Symbol& dst, const Symbol& src) {
            if (src.kind == SymbolKind::LocalVar && !src.reg.empty()) {
                storeSymbol(dst, src.reg);
                return;
            }
            loadSymbol(src, "a0");
            storeSymbol(dst, "a0");
        }

        std::optional<std::int32_t> evalConst(const Expr& expr) const {
            switch (expr.kind) {
                case ExprKind::Number:
                    return toInt32(expr.number);
                case ExprKind::Variable: {
                    const auto symbol = lookup(expr.name);
                    if (symbol && symbol->kind == SymbolKind::Const) {
                        return symbol->constValue;
                    }
                    if (parent_.options_.optimize && peTrackingEnabled()) {
                        if (const auto value = peLookup(expr.name)) {
                            return *value;
                        }
                    }
                    return std::nullopt;
                }
                case ExprKind::Unary: {
                    const auto value = evalConst(*expr.lhs);
                    if (!value) {
                        return std::nullopt;
                    }
                    if (expr.op == "+") {
                        return *value;
                    }
                    if (expr.op == "-") {
                        return toInt32(-static_cast<std::int64_t>(*value));
                    }
                    if (expr.op == "!") {
                        return static_cast<std::int32_t>(*value == 0 ? 1 : 0);
                    }
                    return std::nullopt;
                }
                case ExprKind::Binary:
                    return evalConstBinary(expr);
                case ExprKind::Call:
                    return evalConstCall(expr);
            }
            return std::nullopt;
        }

        std::optional<std::int32_t> evalConstCall(const Expr& expr) const {
            if (!parent_.options_.optimize || !peTrackingEnabled() || peCallDepth_ >= 32) {
                return std::nullopt;
            }
            const auto found = parent_.functions_.find(expr.name);
            if (found == parent_.functions_.end() || !found->second.inlineReturn ||
                !found->second.inlineBlock ||
                found->second.params.size() != expr.args.size()) {
                return std::nullopt;
            }
            const FunctionInfo& info = found->second;

            std::vector<std::int32_t> argValues;
            argValues.reserve(expr.args.size());
            for (const auto& arg : expr.args) {
                const auto value = evalConst(*arg);
                if (!value) {
                    return std::nullopt;
                }
                argValues.push_back(*value);
            }

            ++peCallDepth_;
            pePushScope();
            for (std::size_t i = 0; i < info.params.size(); ++i) {
                peDeclare(info.params[i], argValues[i]);
            }
            for (std::size_t i = 0; i < info.inlineDeclCount; ++i) {
                const Stmt& declStmt = *info.inlineBlock->statements[i];
                if (declStmt.kind != StmtKind::DeclStmt || !declStmt.decl ||
                    !declStmt.decl->init) {
                    pePopScope();
                    --peCallDepth_;
                    return std::nullopt;
                }
                const auto init = evalConst(*declStmt.decl->init);
                if (!init) {
                    pePopScope();
                    --peCallDepth_;
                    return std::nullopt;
                }
                peDeclare(declStmt.decl->name, *init);
            }
            const auto result = evalConst(*info.inlineReturn);
            pePopScope();
            --peCallDepth_;
            return result;
        }

        std::optional<std::int32_t> evalConstBinary(const Expr& expr) const {
            if (expr.op == "&&") {
                const auto lhs = evalConst(*expr.lhs);
                if (!lhs) {
                    return std::nullopt;
                }
                if (*lhs == 0) {
                    return 0;
                }
                const auto rhs = evalConst(*expr.rhs);
                if (!rhs) {
                    return std::nullopt;
                }
                return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
            }
            if (expr.op == "||") {
                const auto lhs = evalConst(*expr.lhs);
                if (!lhs) {
                    return std::nullopt;
                }
                if (*lhs != 0) {
                    return 1;
                }
                const auto rhs = evalConst(*expr.rhs);
                if (!rhs) {
                    return std::nullopt;
                }
                return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
            }

            const auto lhs = evalConst(*expr.lhs);
            const auto rhs = evalConst(*expr.rhs);
            if (!lhs || !rhs) {
                return std::nullopt;
            }

            const std::int64_t left = *lhs;
            const std::int64_t right = *rhs;
            if (expr.op == "+") {
                return toInt32(left + right);
            }
            if (expr.op == "-") {
                return toInt32(left - right);
            }
            if (expr.op == "*") {
                return toInt32(left * right);
            }
            if (expr.op == "/") {
                if (right == 0) {
                    return std::nullopt;
                }
                return toInt32(left / right);
            }
            if (expr.op == "%") {
                if (right == 0) {
                    return std::nullopt;
                }
                return toInt32(left % right);
            }
            if (expr.op == "<") {
                return static_cast<std::int32_t>(left < right ? 1 : 0);
            }
            if (expr.op == ">") {
                return static_cast<std::int32_t>(left > right ? 1 : 0);
            }
            if (expr.op == "<=") {
                return static_cast<std::int32_t>(left <= right ? 1 : 0);
            }
            if (expr.op == ">=") {
                return static_cast<std::int32_t>(left >= right ? 1 : 0);
            }
            if (expr.op == "==") {
                return static_cast<std::int32_t>(left == right ? 1 : 0);
            }
            if (expr.op == "!=") {
                return static_cast<std::int32_t>(left != right ? 1 : 0);
            }
            return std::nullopt;
        }

        void emitStmt(const Stmt& stmt) {
            switch (stmt.kind) {
                case StmtKind::Block:
                    pushScope();
                    if (parent_.options_.optimize) {
                        pePushScope();
                    }
                    for (const auto& child : stmt.statements) {
                        emitStmt(*child);
                        if (parent_.options_.optimize && stmtAlwaysTerminates(*child)) {
                            break;
                        }
                    }
                    if (parent_.options_.optimize) {
                        pePopScope();
                    }
                    popScope();
                    break;
                case StmtKind::Empty:
                    break;
                case StmtKind::ExprStmt:
                    if (!parent_.options_.optimize || exprContainsCall(*stmt.expr)) {
                        emitExpr(*stmt.expr);
                    }
                    break;
                case StmtKind::Assign:
                    emitAssign(stmt);
                    break;
                case StmtKind::DeclStmt:
                    emitDecl(*stmt.decl);
                    break;
                case StmtKind::If:
                    emitIf(stmt);
                    break;
                case StmtKind::While:
                    emitWhile(stmt);
                    break;
                case StmtKind::Break:
                    if (loopStack_.empty()) {
                        fail(stmt.loc, "break outside loop");
                    }
                    body_ << "    jal zero, " << loopStack_.back().second << '\n';
                    break;
                case StmtKind::Continue:
                    if (loopStack_.empty()) {
                        fail(stmt.loc, "continue outside loop");
                    }
                    body_ << "    jal zero, " << loopStack_.back().first << '\n';
                    break;
                case StmtKind::Return:
                    emitReturn(stmt, true);
                    break;
            }
        }

        void emitReturn(const Stmt& stmt, bool jumpToEpilogue) {
            if (stmt.expr && parent_.options_.optimize && emitTailRecursiveReturn(*stmt.expr)) {
                return;
            }
            if (stmt.expr) {
                emitExpr(*stmt.expr);
            } else {
                body_ << "    li a0, 0\n";
            }
            if (jumpToEpilogue) {
                body_ << "    jal zero, " << returnLabel_ << '\n';
            }
        }

        bool emitTailRecursiveReturn(const Expr& expr) {
            if (expr.kind != ExprKind::Call || expr.name != function_.name ||
                expr.args.size() != paramSymbols_.size()) {
                return false;
            }

            pushScope();
            std::vector<Symbol> argSymbols;
            argSymbols.reserve(expr.args.size());
            for (const auto& arg : expr.args) {
                Symbol symbol = allocateLocal();
                if (!symbol.reg.empty()) {
                    emitExprTo(*arg, symbol.reg);
                } else {
                    emitExpr(*arg);
                    storeSymbol(symbol, "a0");
                }
                argSymbols.push_back(symbol);
            }

            for (std::size_t i = 0; i < argSymbols.size(); ++i) {
                copySymbol(paramSymbols_[i], argSymbols[i]);
            }
            popScope();
            body_ << "    jal zero, " << bodyEntryLabel_ << '\n';
            return true;
        }

        void emitDecl(const Decl& decl) {
            pendingPreferredRegisterNames_.erase(decl.name);
            if (decl.isConst) {
                const auto value = evalConst(*decl.init);
                if (!value) {
                    fail(decl.loc, "const initializer is not a compile-time value");
                }
                currentScope()[decl.name] = Symbol{SymbolKind::Const, 0, "", *value, ""};
                return;
            }

            if (parent_.options_.optimize && inlineDepth_ == 0 &&
                referencedNames_.find(decl.name) == referencedNames_.end() &&
                !exprContainsCall(*decl.init)) {
                return;
            }

            if (parent_.options_.optimize && inlineDepth_ == 0 &&
                assignedNames_.find(decl.name) == assignedNames_.end()) {
                if (const auto value = evalConst(*decl.init)) {
                    currentScope()[decl.name] = Symbol{SymbolKind::Const, 0, "", *value, ""};
                    bindPeLocal(decl.name, *decl.init);
                    return;
                }
                if (decl.init->kind == ExprKind::Variable &&
                    assignedNames_.find(decl.init->name) == assignedNames_.end()) {
                    const auto source = lookup(decl.init->name);
                    if (source && source->kind != SymbolKind::GlobalVar) {
                        Symbol alias = *source;
                        alias.ownsLocalRegister = false;
                        currentScope()[decl.name] = std::move(alias);
                        bindPeLocal(decl.name, *decl.init);
                        return;
                    }
                }
            }

            Symbol symbol =
                allocateLocal(inlineDepth_ == 0 ? decl.name : std::string{});
            if (parent_.options_.optimize && !symbol.reg.empty()) {
                emitExprTo(*decl.init, symbol.reg);
                currentScope()[decl.name] = std::move(symbol);
                bindPeLocal(decl.name, *decl.init);
                return;
            }
            emitExpr(*decl.init);
            storeSymbol(symbol, "a0");
            currentScope()[decl.name] = std::move(symbol);
            bindPeLocal(decl.name, *decl.init);
        }

        void emitAssign(const Stmt& stmt) {
            const auto symbol = lookup(stmt.name);
            if (!symbol) {
                fail(stmt.loc, "unknown assignment target: " + stmt.name);
            }
            if (symbol->kind == SymbolKind::Const) {
                fail(stmt.loc, "cannot assign to const: " + stmt.name);
            }

            if (parent_.options_.optimize && peTrackingEnabled()) {
                if (const auto value = evalConst(*stmt.expr)) {
                    peSetKnown(stmt.name, *value);
                } else {
                    peInvalidate(stmt.name);
                }
            }

            if (parent_.options_.optimize && emitOptimizedAssign(stmt, *symbol)) {
                emitCachedInductionUpdates(stmt);
                return;
            }

            emitExpr(*stmt.expr);
            if (symbol->kind == SymbolKind::LocalVar) {
                storeSymbol(*symbol, "a0");
            } else {
                body_ << "    la t0, " << symbol->label << '\n';
                body_ << "    sw a0, 0(t0)\n";
            }
            emitCachedInductionUpdates(stmt);
        }

        bool emitOptimizedAssign(const Stmt& stmt, const Symbol& symbol) {
            if (symbol.kind != SymbolKind::LocalVar || symbol.reg.empty() || !stmt.expr) {
                return false;
            }

            const Expr& expr = *stmt.expr;
            if (isDirectValue(expr)) {
                emitValueTo(expr, symbol.reg);
                return true;
            }

            if (expr.kind != ExprKind::Binary || expr.op == "&&" || expr.op == "||") {
                if (!exprReferencesVariable(expr, stmt.name)) {
                    emitExprTo(expr, symbol.reg);
                    return true;
                }
                return false;
            }
            if (expr.lhs->kind != ExprKind::Variable || expr.lhs->name != stmt.name) {
                if (!exprReferencesVariable(expr, stmt.name)) {
                    emitExprTo(expr, symbol.reg);
                    return true;
                }
                return false;
            }

            const auto rhsConst = evalConst(*expr.rhs);
            if (rhsConst) {
                if ((expr.op == "+" || expr.op == "-") && *rhsConst == 0) {
                    return true;
                }
                if ((expr.op == "*" || expr.op == "/") && *rhsConst == 1) {
                    return true;
                }
                if (expr.op == "*" && *rhsConst == 0) {
                    body_ << "    li " << symbol.reg << ", 0\n";
                    return true;
                }
                if (expr.op == "%" && (*rhsConst == 1 || *rhsConst == -1)) {
                    body_ << "    li " << symbol.reg << ", 0\n";
                    return true;
                }
                if (expr.op == "*") {
                    const auto shift = powerOfTwoShift(*rhsConst);
                    if (shift) {
                        if (*rhsConst < 0 && *shift == 0) {
                            body_ << "    sub " << symbol.reg << ", zero, " << symbol.reg << '\n';
                        } else {
                            body_ << "    slli " << symbol.reg << ", " << symbol.reg << ", " << *shift << '\n';
                            if (*rhsConst < 0) {
                                body_ << "    sub " << symbol.reg << ", zero, " << symbol.reg << '\n';
                            }
                        }
                        return true;
                    }
                    if (emitMultiplyBySparseConstant(symbol.reg, symbol.reg, *rhsConst)) {
                        return true;
                    }
                }
            }
            if (rhsConst && (expr.op == "+" || expr.op == "-")) {
                const int imm = expr.op == "+" ? *rhsConst : -*rhsConst;
                if (fitsI12(imm)) {
                    body_ << "    addi " << symbol.reg << ", " << symbol.reg << ", " << imm << '\n';
                } else {
                    body_ << "    li t0, " << printableI32(*rhsConst) << '\n';
                    if (expr.op == "+") {
                        body_ << "    add " << symbol.reg << ", " << symbol.reg << ", t0\n";
                    } else {
                        body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", t0\n";
                    }
                }
                return true;
            }

            if (const auto cached = cachedLoopExprRegister(*expr.rhs)) {
                emitBinaryOpTo(expr, symbol.reg, symbol.reg, *cached);
                return true;
            }
            if (!isDirectValue(*expr.rhs)) {
                return false;
            }
            const std::string rhsReg = emitValueAsRegister(*expr.rhs, "t0");
            if (expr.op == "+") {
                body_ << "    add " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "-") {
                body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "*") {
                body_ << "    mul " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "/") {
                body_ << "    div " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "%") {
                body_ << "    rem " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "<") {
                body_ << "    slt " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == ">") {
                body_ << "    slt " << symbol.reg << ", " << rhsReg << ", " << symbol.reg << '\n';
            } else if (expr.op == "<=") {
                body_ << "    slt " << symbol.reg << ", " << rhsReg << ", " << symbol.reg << '\n';
                body_ << "    xori " << symbol.reg << ", " << symbol.reg << ", 1\n";
            } else if (expr.op == ">=") {
                body_ << "    slt " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
                body_ << "    xori " << symbol.reg << ", " << symbol.reg << ", 1\n";
            } else if (expr.op == "==") {
                body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
                body_ << "    sltiu " << symbol.reg << ", " << symbol.reg << ", 1\n";
            } else if (expr.op == "!=") {
                body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
                body_ << "    sltu " << symbol.reg << ", zero, " << symbol.reg << '\n';
            } else {
                return false;
            }
            return true;
        }

        void emitIf(const Stmt& stmt) {
            if (parent_.options_.optimize && peTrackingEnabled()) {
                const auto condition = evalConst(*stmt.expr);
                if (condition) {
                    if (*condition != 0) {
                        emitStmt(*stmt.thenBranch);
                    } else if (stmt.elseBranch) {
                        emitStmt(*stmt.elseBranch);
                    }
                    return;
                }
            }

            const std::vector<std::unordered_map<std::string, PeValue>> peSnapshot =
                parent_.options_.optimize ? peScopes_ : std::vector<std::unordered_map<std::string, PeValue>>{};
            const std::string elseLabel = newLabel("else");
            const std::string endLabel = newLabel("endif");
            const bool thenTerminates = stmtAlwaysTerminates(*stmt.thenBranch);
            if (parent_.options_.optimize) {
                emitBranchIfFalse(*stmt.expr, elseLabel);
            } else {
                emitExpr(*stmt.expr);
                body_ << "    beq a0, zero, " << elseLabel << '\n';
            }
            emitStmt(*stmt.thenBranch);
            if (stmt.elseBranch) {
                if (!parent_.options_.optimize || !thenTerminates) {
                    body_ << "    jal zero, " << endLabel << '\n';
                }
                body_ << elseLabel << ":\n";
                emitStmt(*stmt.elseBranch);
                if (!parent_.options_.optimize || !thenTerminates) {
                    body_ << endLabel << ":\n";
                }
            } else {
                body_ << elseLabel << ":\n";
            }
            if (parent_.options_.optimize) {
                peScopes_ = peSnapshot;
            }
        }

        bool peExecStmt(const Stmt& stmt) {
            switch (stmt.kind) {
                case StmtKind::Block: {
                    pePushScope();
                    for (const auto& child : stmt.statements) {
                        if (!peExecStmt(*child)) {
                            pePopScope();
                            return false;
                        }
                    }
                    pePopScope();
                    return true;
                }
                case StmtKind::Empty:
                    return true;
                case StmtKind::ExprStmt:
                    return !exprContainsCall(*stmt.expr);
                case StmtKind::Assign:
                    if (const auto value = evalConst(*stmt.expr)) {
                        peSetKnown(stmt.name, *value);
                    } else {
                        peInvalidate(stmt.name);
                        return false;
                    }
                    return true;
                case StmtKind::DeclStmt:
                    if (stmt.decl->isConst) {
                        if (!evalConst(*stmt.decl->init)) {
                            return false;
                        }
                        peDeclare(stmt.decl->name, *evalConst(*stmt.decl->init));
                        return true;
                    }
                    if (const auto value = evalConst(*stmt.decl->init)) {
                        peDeclare(stmt.decl->name, *value);
                    } else {
                        peDeclareUnknown(stmt.decl->name);
                    }
                    return true;
                case StmtKind::If: {
                    const auto condition = evalConst(*stmt.expr);
                    if (!condition) {
                        return false;
                    }
                    if (*condition != 0) {
                        pePushScope();
                        const bool ok = peExecStmt(*stmt.thenBranch);
                        pePopScope();
                        return ok;
                    }
                    if (stmt.elseBranch) {
                        pePushScope();
                        const bool ok = peExecStmt(*stmt.elseBranch);
                        pePopScope();
                        return ok;
                    }
                    return true;
                }
                case StmtKind::While:
                    // Nested loops: PE-execute inner while with the current env.
                    return tryPeExecuteWhile(stmt);
                case StmtKind::Break:
                case StmtKind::Continue:
                case StmtKind::Return:
                    return false;
            }
            return false;
        }

        bool loopVarsReadyForPe(const Stmt& stmt) const {
            std::unordered_set<std::string> modified;
            collectLoopModifiedNames(*stmt.thenBranch, modified);
            std::unordered_set<std::string> declaredInLoop;
            collectLoopDeclaredNames(*stmt.thenBranch, declaredInLoop);
            for (const std::string& name : modified) {
                // Locals introduced inside the loop are declared during PE exec.
                if (declaredInLoop.find(name) != declaredInLoop.end()) {
                    continue;
                }
                if (!peLookup(name)) {
                    return false;
                }
            }
            return true;
        }

        bool peTimeExceeded() const {
            return peExecDepth_ > 0 &&
                   std::chrono::steady_clock::now() >= peDeadline_;
        }

        // Upper-bound trip estimate for `lhs <op> rhs` when both sides known.
        // Assumes induction steps by at least 1 toward the bound.
        std::optional<std::uint64_t> estimatePeTripUpperBound(const Expr& expr) const {
            if (expr.kind != ExprKind::Binary || !isComparisonOp(expr.op)) {
                return std::nullopt;
            }
            const auto lhs = evalConst(*expr.lhs);
            const auto rhs = evalConst(*expr.rhs);
            if (!lhs || !rhs) {
                return std::nullopt;
            }
            const std::int64_t a = *lhs;
            const std::int64_t b = *rhs;
            if (expr.op == "<") {
                return a < b ? static_cast<std::uint64_t>(b - a) : 0;
            }
            if (expr.op == "<=") {
                return a <= b ? static_cast<std::uint64_t>(b - a + 1) : 0;
            }
            if (expr.op == ">") {
                return a > b ? static_cast<std::uint64_t>(a - b) : 0;
            }
            if (expr.op == ">=") {
                return a >= b ? static_cast<std::uint64_t>(a - b + 1) : 0;
            }
            return std::nullopt;
        }

        bool tryPeExecuteWhile(const Stmt& stmt) {
            if (!parent_.options_.optimize || !stmt.expr || !stmt.thenBranch ||
                !loopVarsReadyForPe(stmt)) {
                return false;
            }
            // Constant-true loops without a foldable exit cannot be PE-executed.
            if (const auto always = evalConst(*stmt.expr)) {
                if (*always != 0) {
                    std::unordered_set<std::string> modified;
                    collectLoopModifiedNames(*stmt.thenBranch, modified);
                    bool conditionDependsOnModified = false;
                    std::function<void(const Expr&)> visit = [&](const Expr& expr) {
                        if (conditionDependsOnModified) {
                            return;
                        }
                        if (expr.kind == ExprKind::Variable &&
                            modified.find(expr.name) != modified.end()) {
                            conditionDependsOnModified = true;
                            return;
                        }
                        if (expr.lhs) {
                            visit(*expr.lhs);
                        }
                        if (expr.rhs) {
                            visit(*expr.rhs);
                        }
                        for (const auto& arg : expr.args) {
                            visit(*arg);
                        }
                    };
                    visit(*stmt.expr);
                    if (!conditionDependsOnModified) {
                        return false;
                    }
                }
            }
            if (const auto trips = estimatePeTripUpperBound(*stmt.expr)) {
                if (*trips > kPeLoopBudget) {
                    return false;
                }
            }
            const auto snapshot = peScopes_;
            const bool outermost = peExecDepth_ == 0;
            if (outermost) {
                peDeadline_ = std::chrono::steady_clock::now() +
                              std::chrono::milliseconds(kPeTimeLimitMilliseconds);
            }
            ++peExecDepth_;
            for (std::uint64_t iter = 0; iter < kPeLoopBudget; ++iter) {
                if ((iter & 65535u) == 0 && peTimeExceeded()) {
                    peScopes_ = snapshot;
                    --peExecDepth_;
                    return false;
                }
                const auto condition = evalConst(*stmt.expr);
                if (!condition) {
                    peScopes_ = snapshot;
                    --peExecDepth_;
                    return false;
                }
                if (*condition == 0) {
                    --peExecDepth_;
                    return true;
                }
                if (!peExecStmt(*stmt.thenBranch)) {
                    peScopes_ = snapshot;
                    --peExecDepth_;
                    return false;
                }
            }
            peScopes_ = snapshot;
            --peExecDepth_;
            return false;
        }

        void emitWhile(const Stmt& stmt) {
            if (parent_.options_.optimize && tryPeExecuteWhile(stmt)) {
                return;
            }

            std::unordered_set<std::string> loopModified;
            collectLoopModifiedNames(*stmt.thenBranch, loopModified);
            // PE may still hold the pre-loop values (e.g. i==0). If we keep them
            // while emitting a real loop, emitValue*/evalConst will constant-fold
            // the condition into `0 < N` and create an infinite loop at runtime.
            if (parent_.options_.optimize) {
                for (const std::string& name : loopModified) {
                    peInvalidate(name);
                }
            }

            std::optional<std::int32_t> constantCondition;
            if (parent_.options_.optimize) {
                constantCondition = evalConst(*stmt.expr);
                if (constantCondition && *constantCondition == 0) {
                    return;
                }
            }

            int cachedExprCount = cacheLoopInvariantExprs(*stmt.thenBranch, 8);
            if (cacheLoopInductionMultiply(*stmt.thenBranch)) {
                ++cachedExprCount;
            }

            const std::string bodyLabel = newLabel("while_body");
            const std::string condLabel = newLabel("while_cond");
            const std::string endLabel = newLabel("while_end");
            const std::string cachedBound =
                cacheLoopComparisonBound(*stmt.expr, loopModified);
            body_ << "    jal zero, " << condLabel << '\n';
            body_ << bodyLabel << ":\n";
            loopStack_.push_back({condLabel, endLabel});
            emitStmt(*stmt.thenBranch);
            loopStack_.pop_back();
            while (cachedExprCount-- > 0) {
                cachedLoopExprs_.pop_back();
            }
            body_ << condLabel << ":\n";
            if (parent_.options_.optimize) {
                if (cachedBound.empty()) {
                    emitBranchIfTrue(*stmt.expr, bodyLabel);
                } else {
                    emitComparisonBranchWithCachedRhs(*stmt.expr, cachedBound, true, bodyLabel);
                }
            } else {
                emitExpr(*stmt.expr);
                body_ << "    bne a0, zero, " << bodyLabel << '\n';
            }
            body_ << endLabel << ":\n";
            if (parent_.options_.optimize) {
                for (const std::string& name : loopModified) {
                    peInvalidate(name);
                }
            }
        }

        void collectLoopModifiedNames(const Stmt& stmt,
                                      std::unordered_set<std::string>& names) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        collectLoopModifiedNames(*child, names);
                    }
                    return;
                case StmtKind::Assign:
                    names.insert(stmt.name);
                    return;
                case StmtKind::DeclStmt:
                    names.insert(stmt.decl->name);
                    return;
                case StmtKind::If:
                    collectLoopModifiedNames(*stmt.thenBranch, names);
                    if (stmt.elseBranch) {
                        collectLoopModifiedNames(*stmt.elseBranch, names);
                    }
                    return;
                case StmtKind::While:
                    collectLoopModifiedNames(*stmt.thenBranch, names);
                    return;
                case StmtKind::Empty:
                case StmtKind::ExprStmt:
                case StmtKind::Break:
                case StmtKind::Continue:
                case StmtKind::Return:
                    return;
            }
        }

        void collectLoopDeclaredNames(const Stmt& stmt,
                                      std::unordered_set<std::string>& names) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        collectLoopDeclaredNames(*child, names);
                    }
                    return;
                case StmtKind::DeclStmt:
                    names.insert(stmt.decl->name);
                    return;
                case StmtKind::If:
                    collectLoopDeclaredNames(*stmt.thenBranch, names);
                    if (stmt.elseBranch) {
                        collectLoopDeclaredNames(*stmt.elseBranch, names);
                    }
                    return;
                case StmtKind::While:
                    collectLoopDeclaredNames(*stmt.thenBranch, names);
                    return;
                case StmtKind::Empty:
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                case StmtKind::Break:
                case StmtKind::Continue:
                case StmtKind::Return:
                    return;
            }
        }

        bool isLoopInvariantExpr(const Expr& expr,
                                 const std::unordered_set<std::string>& modified) const {
            switch (expr.kind) {
                case ExprKind::Number:
                    return true;
                case ExprKind::Variable: {
                    if (modified.find(expr.name) != modified.end()) {
                        return false;
                    }
                    const auto symbol = lookup(expr.name);
                    return symbol && symbol->kind != SymbolKind::GlobalVar;
                }
                case ExprKind::Unary:
                    return isLoopInvariantExpr(*expr.lhs, modified);
                case ExprKind::Binary:
                    return expr.op != "/" && expr.op != "%" &&
                           isLoopInvariantExpr(*expr.lhs, modified) &&
                           isLoopInvariantExpr(*expr.rhs, modified);
                case ExprKind::Call:
                    return false;
            }
            return false;
        }

        std::optional<std::string> cachedLoopExprRegister(const Expr& expr) const {
            for (auto it = cachedLoopExprs_.rbegin(); it != cachedLoopExprs_.rend(); ++it) {
                if (sameExpr(expr, *it->expr)) {
                    return it->reg;
                }
            }
            return std::nullopt;
        }

        void collectInvariantExprs(const Expr& expr,
                                   const std::unordered_set<std::string>& modified,
                                   std::vector<const Expr*>& candidates) const {
            if (candidates.size() >= 16) {
                return;
            }
            if (expr.kind == ExprKind::Binary && !evalConst(expr) &&
                (expr.op == "*" || expr.op == "+" || expr.op == "-") &&
                isLoopInvariantExpr(expr, modified)) {
                candidates.push_back(&expr);
                // Keep walking children so nested invariants are also candidates.
            }
            if (expr.kind == ExprKind::Unary) {
                collectInvariantExprs(*expr.lhs, modified, candidates);
            } else if (expr.kind == ExprKind::Binary) {
                collectInvariantExprs(*expr.lhs, modified, candidates);
                collectInvariantExprs(*expr.rhs, modified, candidates);
            } else if (expr.kind == ExprKind::Call) {
                for (const auto& arg : expr.args) {
                    collectInvariantExprs(*arg, modified, candidates);
                }
            }
        }

        void collectInvariantExprs(const Stmt& stmt,
                                   const std::unordered_set<std::string>& modified,
                                   std::vector<const Expr*>& candidates) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        collectInvariantExprs(*child, modified, candidates);
                    }
                    return;
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                case StmtKind::Return:
                    if (stmt.expr) {
                        collectInvariantExprs(*stmt.expr, modified, candidates);
                    }
                    return;
                case StmtKind::DeclStmt:
                    collectInvariantExprs(*stmt.decl->init, modified, candidates);
                    return;
                case StmtKind::If:
                    collectInvariantExprs(*stmt.expr, modified, candidates);
                    collectInvariantExprs(*stmt.thenBranch, modified, candidates);
                    if (stmt.elseBranch) {
                        collectInvariantExprs(*stmt.elseBranch, modified, candidates);
                    }
                    return;
                case StmtKind::While:
                    collectInvariantExprs(*stmt.expr, modified, candidates);
                    collectInvariantExprs(*stmt.thenBranch, modified, candidates);
                    return;
                case StmtKind::Empty:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return;
            }
        }

        int cacheLoopInvariantExprs(const Stmt& body, int maxCount) {
            if (!parent_.options_.optimize || maxCount <= 0 ||
                localRegCount_ >= allocatableLocalRegisterCount()) {
                return 0;
            }

            std::unordered_set<std::string> modified;
            collectLoopModifiedNames(body, modified);
            std::vector<const Expr*> candidates;
            collectInvariantExprs(body, modified, candidates);

            std::stable_sort(candidates.begin(), candidates.end(),
                             [&](const Expr* lhs, const Expr* rhs) {
                                 const auto leftCount = std::count_if(
                                     candidates.begin(), candidates.end(),
                                     [&](const Expr* other) { return sameExpr(*lhs, *other); });
                                 const auto rightCount = std::count_if(
                                     candidates.begin(), candidates.end(),
                                     [&](const Expr* other) { return sameExpr(*rhs, *other); });
                                 if (leftCount != rightCount) {
                                     return leftCount > rightCount;
                                 }
                                 // Prefer multiplies: they are more expensive than add/sub.
                                 const int leftWeight = lhs->op == "*" ? 2 : 1;
                                 const int rightWeight = rhs->op == "*" ? 2 : 1;
                                 return leftWeight > rightWeight;
                             });

            int cached = 0;
            for (const Expr* candidate : candidates) {
                if (cached >= maxCount || localRegCount_ >= allocatableLocalRegisterCount()) {
                    break;
                }
                const auto occurrences = std::count_if(
                    candidates.begin(), candidates.end(),
                    [&](const Expr* other) { return sameExpr(*candidate, *other); });
                // Always hoist multiplies; hoist add/sub only when reused.
                if (candidate->op != "*" && occurrences < 2) {
                    continue;
                }
                if (cachedLoopExprRegister(*candidate)) {
                    continue;
                }
                Symbol symbol = allocateLocal();
                if (symbol.reg.empty()) {
                    break;
                }
                emitExprTo(*candidate, symbol.reg);
                cachedLoopExprs_.push_back(CachedLoopExpr{candidate, symbol.reg, {}, nullptr});
                ++cached;
            }
            return cached;
        }

        void collectInvariantMultiplies(const Expr& expr,
                                        const std::unordered_set<std::string>& modified,
                                        std::vector<const Expr*>& candidates) const {
            collectInvariantExprs(expr, modified, candidates);
        }

        void collectInvariantMultiplies(const Stmt& stmt,
                                        const std::unordered_set<std::string>& modified,
                                        std::vector<const Expr*>& candidates) const {
            collectInvariantExprs(stmt, modified, candidates);
        }

        int cacheLoopInvariantMultiplies(const Stmt& body, int maxCount) {
            return cacheLoopInvariantExprs(body, maxCount);
        }

        bool isUnitIncrement(const Stmt& stmt, const std::string& name) const {
            if (stmt.kind != StmtKind::Assign || stmt.name != name || !stmt.expr ||
                stmt.expr->kind != ExprKind::Binary || stmt.expr->op != "+") {
                return false;
            }
            const Expr* step = nullptr;
            if (stmt.expr->lhs->kind == ExprKind::Variable && stmt.expr->lhs->name == name) {
                step = stmt.expr->rhs.get();
            } else if (stmt.expr->rhs->kind == ExprKind::Variable &&
                       stmt.expr->rhs->name == name) {
                step = stmt.expr->lhs.get();
            }
            const auto value = step ? evalConst(*step) : std::nullopt;
            return value && *value == 1;
        }

        void inspectInductionUpdates(const Stmt& stmt, const std::string& name,
                                     int& assignments, const Stmt*& unitUpdate,
                                     bool& redeclared) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        inspectInductionUpdates(*child, name, assignments, unitUpdate, redeclared);
                    }
                    return;
                case StmtKind::Assign:
                    if (stmt.name == name) {
                        ++assignments;
                        unitUpdate = isUnitIncrement(stmt, name) ? &stmt : nullptr;
                    }
                    return;
                case StmtKind::DeclStmt:
                    redeclared = redeclared || stmt.decl->name == name;
                    return;
                case StmtKind::If:
                    inspectInductionUpdates(
                        *stmt.thenBranch, name, assignments, unitUpdate, redeclared);
                    if (stmt.elseBranch) {
                        inspectInductionUpdates(
                            *stmt.elseBranch, name, assignments, unitUpdate, redeclared);
                    }
                    return;
                case StmtKind::While:
                    inspectInductionUpdates(
                        *stmt.thenBranch, name, assignments, unitUpdate, redeclared);
                    return;
                case StmtKind::Empty:
                case StmtKind::ExprStmt:
                case StmtKind::Break:
                case StmtKind::Continue:
                case StmtKind::Return:
                    return;
            }
        }

        void collectInductionMultiplies(
            const Expr& expr, const std::unordered_set<std::string>& modified,
            std::vector<InductionMultiplyCandidate>& candidates) const {
            if (candidates.size() >= 8) {
                return;
            }
            if (expr.kind == ExprKind::Binary && expr.op == "*") {
                const Expr* induction = nullptr;
                const Expr* stride = nullptr;
                if (expr.lhs->kind == ExprKind::Variable && isDirectValue(*expr.rhs)) {
                    induction = expr.lhs.get();
                    stride = expr.rhs.get();
                } else if (expr.rhs->kind == ExprKind::Variable && isDirectValue(*expr.lhs)) {
                    induction = expr.rhs.get();
                    stride = expr.lhs.get();
                }
                if (induction && isLoopInvariantExpr(*stride, modified)) {
                    candidates.push_back(
                        InductionMultiplyCandidate{&expr, induction->name, stride});
                    return;
                }
            }
            if (expr.kind == ExprKind::Unary) {
                collectInductionMultiplies(*expr.lhs, modified, candidates);
            } else if (expr.kind == ExprKind::Binary) {
                collectInductionMultiplies(*expr.lhs, modified, candidates);
                collectInductionMultiplies(*expr.rhs, modified, candidates);
            } else if (expr.kind == ExprKind::Call) {
                for (const auto& arg : expr.args) {
                    collectInductionMultiplies(*arg, modified, candidates);
                }
            }
        }

        void collectInductionMultiplies(
            const Stmt& stmt, const std::unordered_set<std::string>& modified,
            std::vector<InductionMultiplyCandidate>& candidates) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        collectInductionMultiplies(*child, modified, candidates);
                    }
                    return;
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                case StmtKind::Return:
                    if (stmt.expr) collectInductionMultiplies(*stmt.expr, modified, candidates);
                    return;
                case StmtKind::DeclStmt:
                    collectInductionMultiplies(*stmt.decl->init, modified, candidates);
                    return;
                case StmtKind::If:
                    collectInductionMultiplies(*stmt.expr, modified, candidates);
                    collectInductionMultiplies(*stmt.thenBranch, modified, candidates);
                    if (stmt.elseBranch) {
                        collectInductionMultiplies(*stmt.elseBranch, modified, candidates);
                    }
                    return;
                case StmtKind::While:
                    collectInductionMultiplies(*stmt.expr, modified, candidates);
                    collectInductionMultiplies(*stmt.thenBranch, modified, candidates);
                    return;
                case StmtKind::Empty:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return;
            }
        }

        bool cacheLoopInductionMultiply(const Stmt& body) {
            if (!parent_.options_.optimize ||
                localRegCount_ >= allocatableLocalRegisterCount()) return false;
            std::unordered_set<std::string> modified;
            collectLoopModifiedNames(body, modified);
            std::vector<InductionMultiplyCandidate> candidates;
            collectInductionMultiplies(body, modified, candidates);
            for (const auto& candidate : candidates) {
                const auto occurrences = std::count_if(
                    candidates.begin(), candidates.end(), [&](const auto& other) {
                        return sameExpr(*candidate.product, *other.product);
                    });
                int assignments = 0;
                const Stmt* unitUpdate = nullptr;
                bool redeclared = false;
                inspectInductionUpdates(body, candidate.inductionName,
                                         assignments, unitUpdate, redeclared);
                const auto symbol = lookup(candidate.inductionName);
                const auto strideValue = evalConst(*candidate.stride);
                if (occurrences < 2 || assignments != 1 || !unitUpdate || redeclared || !symbol ||
                    symbol->kind != SymbolKind::LocalVar ||
                    (strideValue && (*strideValue == 0 || *strideValue == 1 ||
                                     *strideValue == -1)) ||
                    cachedLoopExprRegister(*candidate.product)) {
                    continue;
                }
                Symbol cached = allocateLocal();
                if (cached.reg.empty()) return false;
                emitExprTo(*candidate.product, cached.reg);
                cachedLoopExprs_.push_back(CachedLoopExpr{
                    candidate.product, cached.reg, candidate.inductionName, candidate.stride});
                return true;
            }
            return false;
        }

        void emitCachedInductionUpdates(const Stmt& assignment) {
            if (!parent_.options_.optimize) return;
            for (const CachedLoopExpr& cached : cachedLoopExprs_) {
                if (cached.inductionName != assignment.name || !cached.stride) continue;
                if (const auto stride = evalConst(*cached.stride)) {
                    if (fitsI12(*stride)) {
                        body_ << "    addi " << cached.reg << ", " << cached.reg << ", "
                              << printableI32(*stride) << '\n';
                    } else {
                        body_ << "    li t0, " << printableI32(*stride) << '\n';
                        body_ << "    add " << cached.reg << ", " << cached.reg << ", t0\n";
                    }
                } else {
                    const std::string strideReg = emitValueAsRegister(*cached.stride, "t0");
                    body_ << "    add " << cached.reg << ", " << cached.reg << ", "
                          << strideReg << '\n';
                }
            }
        }
        std::string cacheLoopComparisonBound(
            const Expr& expr, const std::unordered_set<std::string>& loopModified) {
            if (!parent_.options_.optimize || expr.kind != ExprKind::Binary ||
                !isComparisonOp(expr.op)) {
                return {};
            }
            // Prefer caching a loop-invariant RHS so the hot comparison avoids reloads.
            if (evalConst(*expr.lhs)) {
                return {};
            }
            if (const auto rhs = evalConst(*expr.rhs)) {
                if (*rhs == 0 || localRegCount_ >= allocatableLocalRegisterCount()) {
                    return {};
                }
                Symbol cached = allocateLocal();
                if (cached.reg.empty()) {
                    return {};
                }
                body_ << "    li " << cached.reg << ", " << printableI32(*rhs) << '\n';
                return cached.reg;
            }
            if (expr.rhs->kind != ExprKind::Variable ||
                loopModified.find(expr.rhs->name) != loopModified.end()) {
                return {};
            }
            const auto symbol = lookup(expr.rhs->name);
            if (!symbol || symbol->kind == SymbolKind::GlobalVar) {
                return {};
            }
            if (!symbol->reg.empty()) {
                return symbol->reg;
            }
            if (symbol->kind == SymbolKind::Const) {
                if (localRegCount_ >= allocatableLocalRegisterCount()) {
                    return {};
                }
                Symbol cached = allocateLocal();
                if (cached.reg.empty()) {
                    return {};
                }
                body_ << "    li " << cached.reg << ", "
                      << printableI32(symbol->constValue) << '\n';
                return cached.reg;
            }
            if (localRegCount_ >= allocatableLocalRegisterCount()) {
                return {};
            }
            Symbol cached = allocateLocal();
            if (cached.reg.empty()) {
                return {};
            }
            loadSymbol(*symbol, cached.reg);
            return cached.reg;
        }

        void emitComparisonBranchWithCachedRhs(const Expr& expr, const std::string& rhsReg,
                                               bool branchIfTrue, const std::string& label) {
            std::string lhsReg;
            if (isDirectValue(*expr.lhs)) {
                lhsReg = emitValueAsRegister(*expr.lhs, "t5");
            } else {
                emitExpr(*expr.lhs);
                lhsReg = "a0";
            }
            emitComparisonJump(expr.op, lhsReg, rhsReg, branchIfTrue, label);
        }
        void emitExpr(const Expr& expr) {
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value) {
                    body_ << "    li a0, " << printableI32(*value) << '\n';
                    return;
                }
                if (const auto cached = cachedLoopExprRegister(expr)) {
                    emitMove("a0", *cached);
                    return;
                }
            }

            switch (expr.kind) {
                case ExprKind::Number:
                    body_ << "    li a0, " << printableI32(toInt32(expr.number)) << '\n';
                    break;
                case ExprKind::Variable:
                    emitVariable(expr);
                    break;
                case ExprKind::Unary:
                    emitUnary(expr);
                    break;
                case ExprKind::Binary:
                    emitBinary(expr);
                    break;
                case ExprKind::Call:
                    emitCall(expr);
                    break;
            }
        }

        void emitExprTo(const Expr& expr, const std::string& dst) {
            if (dst == "a0") {
                emitExpr(expr);
                return;
            }
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value) {
                    body_ << "    li " << dst << ", " << printableI32(*value) << '\n';
                    return;
                }
                if (const auto cached = cachedLoopExprRegister(expr)) {
                    emitMove(dst, *cached);
                    return;
                }
            }

            switch (expr.kind) {
                case ExprKind::Number:
                case ExprKind::Variable:
                    emitValueTo(expr, dst);
                    return;
                case ExprKind::Unary:
                    if (emitUnaryTo(expr, dst)) {
                        return;
                    }
                    break;
                case ExprKind::Binary:
                    if (expr.op != "&&" && expr.op != "||" && emitBinaryTo(expr, dst)) {
                        return;
                    }
                    break;
                case ExprKind::Call:
                    break;
            }

            emitExpr(expr);
            emitMove(dst, "a0");
        }

        void emitVariable(const Expr& expr) {
            const auto symbol = lookup(expr.name);
            if (!symbol) {
                fail(expr.loc, "unknown variable: " + expr.name);
            }
            if (symbol->kind == SymbolKind::Const) {
                body_ << "    li a0, " << printableI32(symbol->constValue) << '\n';
            } else if (symbol->kind == SymbolKind::LocalVar) {
                loadSymbol(*symbol, "a0");
            } else {
                body_ << "    la t0, " << symbol->label << '\n';
                body_ << "    lw a0, 0(t0)\n";
            }
        }

        void emitUnary(const Expr& expr) {
            emitExpr(*expr.lhs);
            if (expr.op == "+") {
                return;
            }
            if (expr.op == "-") {
                body_ << "    sub a0, zero, a0\n";
                return;
            }
            if (expr.op == "!") {
                body_ << "    sltiu a0, a0, 1\n";
                return;
            }
            fail(expr.loc, "unknown unary operator");
        }

        bool emitUnaryTo(const Expr& expr, const std::string& dst) {
            emitExprTo(*expr.lhs, dst);
            if (expr.op == "+") {
                return true;
            }
            if (expr.op == "-") {
                body_ << "    sub " << dst << ", zero, " << dst << '\n';
                return true;
            }
            if (expr.op == "!") {
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
                return true;
            }
            return false;
        }

        bool exprUsesOnlyConstSymbols(const Expr& expr) const {
            switch (expr.kind) {
                case ExprKind::Number:
                    return true;
                case ExprKind::Variable: {
                    const auto symbol = lookup(expr.name);
                    return symbol && symbol->kind == SymbolKind::Const;
                }
                case ExprKind::Unary:
                    return exprUsesOnlyConstSymbols(*expr.lhs);
                case ExprKind::Binary:
                    return exprUsesOnlyConstSymbols(*expr.lhs) &&
                           exprUsesOnlyConstSymbols(*expr.rhs);
                case ExprKind::Call:
                    return false;
            }
            return false;
        }

        void emitBranchIfFalse(const Expr& expr, const std::string& falseLabel) {
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value && exprUsesOnlyConstSymbols(expr)) {
                    if (*value == 0) {
                        body_ << "    jal zero, " << falseLabel << '\n';
                    }
                    return;
                }
            }

            if (expr.kind == ExprKind::Unary && expr.op == "!") {
                emitBranchIfTrue(*expr.lhs, falseLabel);
                return;
            }

            if (expr.kind == ExprKind::Binary) {
                if (expr.op == "&&") {
                    emitBranchIfFalse(*expr.lhs, falseLabel);
                    emitBranchIfFalse(*expr.rhs, falseLabel);
                    return;
                }
                if (expr.op == "||") {
                    const std::string trueLabel = newLabel("or_true");
                    emitBranchIfTrue(*expr.lhs, trueLabel);
                    emitBranchIfFalse(*expr.rhs, falseLabel);
                    body_ << trueLabel << ":\n";
                    return;
                }
                if (emitComparisonBranch(expr, false, falseLabel)) {
                    return;
                }
            }

            emitExpr(expr);
            body_ << "    beq a0, zero, " << falseLabel << '\n';
        }

        void emitBranchIfTrue(const Expr& expr, const std::string& trueLabel) {
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value && exprUsesOnlyConstSymbols(expr)) {
                    if (*value != 0) {
                        body_ << "    jal zero, " << trueLabel << '\n';
                    }
                    return;
                }
            }

            if (expr.kind == ExprKind::Unary && expr.op == "!") {
                emitBranchIfFalse(*expr.lhs, trueLabel);
                return;
            }

            if (expr.kind == ExprKind::Binary) {
                if (expr.op == "&&") {
                    const std::string falseLabel = newLabel("and_false");
                    emitBranchIfFalse(*expr.lhs, falseLabel);
                    emitBranchIfTrue(*expr.rhs, trueLabel);
                    body_ << falseLabel << ":\n";
                    return;
                }
                if (expr.op == "||") {
                    emitBranchIfTrue(*expr.lhs, trueLabel);
                    emitBranchIfTrue(*expr.rhs, trueLabel);
                    return;
                }
                if (emitComparisonBranch(expr, true, trueLabel)) {
                    return;
                }
            }

            emitExpr(expr);
            body_ << "    bne a0, zero, " << trueLabel << '\n';
        }

        void emitBinary(const Expr& expr) {
            if (expr.op == "&&") {
                emitLogicalAnd(expr);
                return;
            }
            if (expr.op == "||") {
                emitLogicalOr(expr);
                return;
            }
            if (parent_.options_.optimize && emitCachedOperandBinaryTo(expr, "a0")) {
                return;
            }
            if (parent_.options_.optimize && emitRepeatedOperandBinary(expr)) {
                return;
            }
            if (parent_.options_.optimize && emitImmediateBinary(expr)) {
                return;
            }
            if (parent_.options_.optimize && emitDirectValueBinary(expr)) {
                return;
            }

            emitExpr(*expr.lhs);
            pushA0(exprContainsRuntimeCall(*expr.rhs));
            emitExpr(*expr.rhs);
            const std::string lhsReg = popValue("t0");

            if (expr.op == "+") {
                body_ << "    add a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "-") {
                body_ << "    sub a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "*") {
                body_ << "    mul a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "/") {
                body_ << "    div a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "%") {
                body_ << "    rem a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "<") {
                body_ << "    slt a0, " << lhsReg << ", a0\n";
            } else if (expr.op == ">") {
                body_ << "    slt a0, a0, " << lhsReg << '\n';
            } else if (expr.op == "<=") {
                body_ << "    slt a0, a0, " << lhsReg << '\n';
                body_ << "    xori a0, a0, 1\n";
            } else if (expr.op == ">=") {
                body_ << "    slt a0, " << lhsReg << ", a0\n";
                body_ << "    xori a0, a0, 1\n";
            } else if (expr.op == "==") {
                body_ << "    sub a0, " << lhsReg << ", a0\n";
                body_ << "    sltiu a0, a0, 1\n";
            } else if (expr.op == "!=") {
                body_ << "    sub a0, " << lhsReg << ", a0\n";
                body_ << "    sltu a0, zero, a0\n";
            } else {
                fail(expr.loc, "unknown binary operator");
            }
        }

        bool isDirectValue(const Expr& expr) const {
            return expr.kind == ExprKind::Number || expr.kind == ExprKind::Variable;
        }

        void emitValueTo(const Expr& expr, const std::string& reg) {
            if (parent_.options_.optimize) {
                if (const auto value = evalConst(expr)) {
                    body_ << "    li " << reg << ", " << printableI32(*value) << '\n';
                    return;
                }
            }
            if (expr.kind == ExprKind::Number) {
                body_ << "    li " << reg << ", " << printableI32(toInt32(expr.number)) << '\n';
                return;
            }
            if (expr.kind == ExprKind::Variable) {
                const auto symbol = lookup(expr.name);
                if (!symbol) {
                    fail(expr.loc, "unknown variable: " + expr.name);
                }
                loadSymbol(*symbol, reg);
                return;
            }
            throw std::runtime_error("internal error: unsupported direct value");
        }

        bool emitCachedOperandBinaryTo(const Expr& expr, const std::string& dst) {
            if (!parent_.options_.optimize || !isValueBinaryOp(expr.op)) {
                return false;
            }
            const auto lhsCached = cachedLoopExprRegister(*expr.lhs);
            const auto rhsCached = cachedLoopExprRegister(*expr.rhs);
            if (!lhsCached && !rhsCached) {
                return false;
            }
            if (lhsCached && rhsCached) {
                emitBinaryOpTo(expr, dst, *lhsCached, *rhsCached);
            } else if (rhsCached) {
                emitExprTo(*expr.lhs, dst);
                emitBinaryOpTo(expr, dst, dst, *rhsCached);
            } else {
                emitExprTo(*expr.rhs, dst);
                emitBinaryOpTo(expr, dst, *lhsCached, dst);
            }
            return true;
        }

        bool emitBinaryTo(const Expr& expr, const std::string& dst) {
            if (emitCachedOperandBinaryTo(expr, dst)) {
                return true;
            }
            if (emitRepeatedOperandBinaryTo(expr, dst)) {
                return true;
            }
            if (emitImmediateBinaryTo(expr, dst)) {
                return true;
            }
            if (emitDirectValueBinaryTo(expr, dst)) {
                return true;
            }
            return false;
        }

        std::string emitValueAsRegister(const Expr& expr, const std::string& scratchReg) {
            if (parent_.options_.optimize) {
                if (const auto value = evalConst(expr)) {
                    body_ << "    li " << scratchReg << ", " << printableI32(*value) << '\n';
                    return scratchReg;
                }
            }
            if (expr.kind == ExprKind::Number) {
                body_ << "    li " << scratchReg << ", " << printableI32(toInt32(expr.number)) << '\n';
                return scratchReg;
            }
            if (expr.kind == ExprKind::Variable) {
                const auto symbol = lookup(expr.name);
                if (!symbol) {
                    fail(expr.loc, "unknown variable: " + expr.name);
                }
                if (symbol->kind == SymbolKind::LocalVar && !symbol->reg.empty()) {
                    return symbol->reg;
                }
                loadSymbol(*symbol, scratchReg);
                return scratchReg;
            }
            throw std::runtime_error("internal error: unsupported direct value");
        }

        void emitBinaryOpTo(const Expr& expr, const std::string& dst, const std::string& lhsReg,
                            const std::string& rhsReg) {
            if (expr.op == "+") {
                body_ << "    add " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "-") {
                body_ << "    sub " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "*") {
                body_ << "    mul " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "/") {
                body_ << "    div " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "%") {
                body_ << "    rem " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "<") {
                body_ << "    slt " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == ">") {
                body_ << "    slt " << dst << ", " << rhsReg << ", " << lhsReg << '\n';
            } else if (expr.op == "<=") {
                body_ << "    slt " << dst << ", " << rhsReg << ", " << lhsReg << '\n';
                body_ << "    xori " << dst << ", " << dst << ", 1\n";
            } else if (expr.op == ">=") {
                body_ << "    slt " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
                body_ << "    xori " << dst << ", " << dst << ", 1\n";
            } else if (expr.op == "==") {
                body_ << "    sub " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
            } else if (expr.op == "!=") {
                body_ << "    sub " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
                body_ << "    sltu " << dst << ", zero, " << dst << '\n';
            }
        }

        bool sameExpr(const Expr& lhs, const Expr& rhs) const {
            if (lhs.kind != rhs.kind) {
                return false;
            }
            switch (lhs.kind) {
                case ExprKind::Number:
                    return toInt32(lhs.number) == toInt32(rhs.number);
                case ExprKind::Variable:
                    return lhs.name == rhs.name;
                case ExprKind::Unary:
                    return lhs.op == rhs.op && sameExpr(*lhs.lhs, *rhs.lhs);
                case ExprKind::Binary:
                    return lhs.op == rhs.op && sameExpr(*lhs.lhs, *rhs.lhs) &&
                           sameExpr(*lhs.rhs, *rhs.rhs);
                case ExprKind::Call:
                    if (lhs.name != rhs.name || lhs.args.size() != rhs.args.size()) {
                        return false;
                    }
                    for (std::size_t i = 0; i < lhs.args.size(); ++i) {
                        if (!sameExpr(*lhs.args[i], *rhs.args[i])) {
                            return false;
                        }
                    }
                    return true;
            }
            return false;
        }

        bool exprReferencesVariable(const Expr& expr, const std::string& name) const {
            switch (expr.kind) {
                case ExprKind::Number:
                    return false;
                case ExprKind::Variable:
                    return expr.name == name;
                case ExprKind::Unary:
                    return exprReferencesVariable(*expr.lhs, name);
                case ExprKind::Binary:
                    return exprReferencesVariable(*expr.lhs, name) ||
                           exprReferencesVariable(*expr.rhs, name);
                case ExprKind::Call:
                    for (const auto& arg : expr.args) {
                        if (exprReferencesVariable(*arg, name)) {
                            return true;
                        }
                    }
                    return false;
            }
            return false;
        }

        bool exprReferencesAnyName(const Expr& expr,
                                   const std::unordered_set<std::string>& names) const {
            for (const std::string& name : names) {
                if (exprReferencesVariable(expr, name)) {
                    return true;
                }
            }
            return false;
        }

        bool emitRepeatedOperandBinary(const Expr& expr) {
            return emitRepeatedOperandBinaryTo(expr, "a0");
        }

        bool emitRepeatedOperandBinaryTo(const Expr& expr, const std::string& dst) {
            if (!sameExpr(*expr.lhs, *expr.rhs) || exprContainsCall(*expr.lhs)) {
                return false;
            }

            if (expr.op == "+") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    slli " << dst << ", " << dst << ", 1\n";
                return true;
            }
            if (expr.op == "-") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "*") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    mul " << dst << ", " << dst << ", " << dst << '\n';
                return true;
            }
            if (expr.op == "<" || expr.op == ">" || expr.op == "!=") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "<=" || expr.op == ">=" || expr.op == "==") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    li " << dst << ", 1\n";
                return true;
            }
            return false;
        }

        void emitMove(const std::string& dst, const std::string& src) {
            if (dst != src) {
                body_ << "    addi " << dst << ", " << src << ", 0\n";
            }
        }

        std::string chooseScratch(std::initializer_list<std::string> avoid) const {
            for (const char* candidateName : {"t0", "t5", "t6"}) {
                const std::string candidate(candidateName);
                bool used = false;
                for (const std::string& reg : avoid) {
                    if (candidate == reg) {
                        used = true;
                        break;
                    }
                }
                if (!used) {
                    return candidate;
                }
            }
            return "t6";
        }

        void emitShiftedTerm(const std::string& dst, const std::string& baseReg, int shift) {
            if (shift == 0) {
                emitMove(dst, baseReg);
            } else {
                body_ << "    slli " << dst << ", " << baseReg << ", " << shift << '\n';
            }
        }

        bool emitMultiplyBySparseConstant(const std::string& dst, const std::string& lhsReg, int imm) {
            const std::int64_t value = imm;
            std::uint64_t magnitude = value < 0 ? static_cast<std::uint64_t>(-value)
                                                : static_cast<std::uint64_t>(value);
            if (magnitude == 0 || (magnitude & (magnitude - 1)) == 0) {
                return false;
            }

            std::vector<int> shifts;
            for (int bit = 0; bit < 31; ++bit) {
                if ((magnitude & (1ull << bit)) != 0) {
                    shifts.push_back(bit);
                }
            }
            // 2-4 set bits: shift-add is cheaper than mul on the evaluation target.
            if (shifts.size() < 2 || shifts.size() > 4) {
                return false;
            }

            std::string baseReg = lhsReg;
            std::string scratch = chooseScratch({dst, lhsReg});
            if (dst == lhsReg) {
                emitMove(scratch, lhsReg);
                baseReg = scratch;
                scratch = chooseScratch({dst, baseReg});
            }

            emitShiftedTerm(dst, baseReg, shifts[0]);
            for (std::size_t i = 1; i < shifts.size(); ++i) {
                emitShiftedTerm(scratch, baseReg, shifts[i]);
                body_ << "    add " << dst << ", " << dst << ", " << scratch << '\n';
            }
            if (value < 0) {
                body_ << "    sub " << dst << ", zero, " << dst << '\n';
            }
            return true;
        }

        bool isComparisonOp(const std::string& op) const {
            return op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=";
        }

        void emitComparisonJump(const std::string& op, const std::string& lhsReg,
                                const std::string& rhsReg, bool branchIfTrue,
                                const std::string& label) {
            if (branchIfTrue) {
                if (op == "<") {
                    body_ << "    blt " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                } else if (op == ">") {
                    body_ << "    blt " << rhsReg << ", " << lhsReg << ", " << label << '\n';
                } else if (op == "<=") {
                    body_ << "    bge " << rhsReg << ", " << lhsReg << ", " << label << '\n';
                } else if (op == ">=") {
                    body_ << "    bge " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                } else if (op == "==") {
                    body_ << "    beq " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                } else if (op == "!=") {
                    body_ << "    bne " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                }
                return;
            }

            if (op == "<") {
                body_ << "    bge " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            } else if (op == ">") {
                body_ << "    bge " << rhsReg << ", " << lhsReg << ", " << label << '\n';
            } else if (op == "<=") {
                body_ << "    blt " << rhsReg << ", " << lhsReg << ", " << label << '\n';
            } else if (op == ">=") {
                body_ << "    blt " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            } else if (op == "==") {
                body_ << "    bne " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            } else if (op == "!=") {
                body_ << "    beq " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            }
        }

        bool emitComparisonBranch(const Expr& expr, bool branchIfTrue, const std::string& label) {
            if (!isComparisonOp(expr.op)) {
                return false;
            }

            if (isDirectValue(*expr.lhs) && isDirectValue(*expr.rhs)) {
                const bool lhsZero = expr.lhs->kind == ExprKind::Number && toInt32(expr.lhs->number) == 0;
                const bool rhsZero = expr.rhs->kind == ExprKind::Number && toInt32(expr.rhs->number) == 0;
                const std::string lhsReg = lhsZero ? "zero" : emitValueAsRegister(*expr.lhs, "t5");
                const std::string rhsReg = rhsZero ? "zero" : emitValueAsRegister(*expr.rhs, "t0");
                emitComparisonJump(expr.op, lhsReg, rhsReg, branchIfTrue, label);
                return true;
            }

            emitExpr(*expr.lhs);
            pushA0(exprContainsRuntimeCall(*expr.rhs));
            emitExpr(*expr.rhs);
            const std::string lhsReg = popValue("t0");
            emitComparisonJump(expr.op, lhsReg, "a0", branchIfTrue, label);
            return true;
        }

        bool isValueBinaryOp(const std::string& op) const {
            return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
                   isComparisonOp(op);
        }

        bool emitDirectValueBinaryTo(const Expr& expr, const std::string& dst) {
            if (!isValueBinaryOp(expr.op) || !isDirectValue(*expr.lhs) || !isDirectValue(*expr.rhs)) {
                return false;
            }

            const std::string lhsScratch = dst == "t0" ? "t5" : dst;
            const std::string rhsScratch = dst == "t0" ? "t6" : "t0";
            const std::string lhsReg = emitValueAsRegister(*expr.lhs, lhsScratch);
            const std::string rhsReg = emitValueAsRegister(*expr.rhs, rhsScratch);
            emitBinaryOpTo(expr, dst, lhsReg, rhsReg);
            return true;
        }

        bool emitDirectValueBinary(const Expr& expr) {
            return emitDirectValueBinaryTo(expr, "a0");
        }

        bool emitImmediateBinaryTo(const Expr& expr, const std::string& dst) {
            if (!isValueBinaryOp(expr.op)) {
                return false;
            }
            const auto rhs = evalConst(*expr.rhs);
            if (!rhs) {
                return false;
            }
            const int imm = *rhs;
            std::string lhsReg = dst;
            if (isDirectValue(*expr.lhs)) {
                lhsReg = emitValueAsRegister(*expr.lhs, dst);
            } else {
                emitExpr(*expr.lhs);
                lhsReg = "a0";
            }
            const std::string immReg = lhsReg == "t0" ? "t5" : "t0";

            if ((expr.op == "+" || expr.op == "-") && imm == 0) {
                emitMove(dst, lhsReg);
                return true;
            }
            if ((expr.op == "*" || expr.op == "/") && imm == 1) {
                emitMove(dst, lhsReg);
                return true;
            }
            if (expr.op == "*" && imm == 0) {
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "%" && (imm == 1 || imm == -1)) {
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "*") {
                const auto shift = powerOfTwoShift(imm);
                if (shift) {
                    if (imm < 0 && *shift == 0) {
                        body_ << "    sub " << dst << ", zero, " << lhsReg << '\n';
                    } else {
                        body_ << "    slli " << dst << ", " << lhsReg << ", " << *shift << '\n';
                        if (imm < 0) {
                            body_ << "    sub " << dst << ", zero, " << dst << '\n';
                        }
                    }
                    return true;
                }
                if (emitMultiplyBySparseConstant(dst, lhsReg, imm)) {
                    return true;
                }
            }

            if (expr.op == "+" && fitsI12(imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << imm << '\n';
                return true;
            }
            if (expr.op == "+") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    add " << dst << ", " << lhsReg << ", " << immReg << '\n';
                return true;
            }
            if (expr.op == "-" && fitsI12(-imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << -imm << '\n';
                return true;
            }
            if (expr.op == "-") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    sub " << dst << ", " << lhsReg << ", " << immReg << '\n';
                return true;
            }
            if (expr.op == "<" && fitsI12(imm)) {
                body_ << "    slti " << dst << ", " << lhsReg << ", " << imm << '\n';
                return true;
            }
            if (expr.op == "<") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    slt " << dst << ", " << lhsReg << ", " << immReg << '\n';
                return true;
            }
            if (expr.op == "==" && fitsI12(-imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << -imm << '\n';
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
                return true;
            }
            if (expr.op == "==") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    sub " << dst << ", " << lhsReg << ", " << immReg << '\n';
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
                return true;
            }
            if (expr.op == "!=" && fitsI12(-imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << -imm << '\n';
                body_ << "    sltu " << dst << ", zero, " << dst << '\n';
                return true;
            }
            if (expr.op == "!=") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    sub " << dst << ", " << lhsReg << ", " << immReg << '\n';
                body_ << "    sltu " << dst << ", zero, " << dst << '\n';
                return true;
            }
            if (expr.op == "*" || expr.op == "/" || expr.op == "%" || expr.op == "<=" ||
                expr.op == ">" || expr.op == ">=") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                if (expr.op == "*") {
                    body_ << "    mul " << dst << ", " << lhsReg << ", " << immReg << '\n';
                } else if (expr.op == "/") {
                    body_ << "    div " << dst << ", " << lhsReg << ", " << immReg << '\n';
                } else if (expr.op == "%") {
                    body_ << "    rem " << dst << ", " << lhsReg << ", " << immReg << '\n';
                } else if (expr.op == "<=") {
                    body_ << "    slt " << dst << ", " << immReg << ", " << lhsReg << '\n';
                    body_ << "    xori " << dst << ", " << dst << ", 1\n";
                } else if (expr.op == ">") {
                    body_ << "    slt " << dst << ", " << immReg << ", " << lhsReg << '\n';
                } else {
                    body_ << "    slt " << dst << ", " << lhsReg << ", " << immReg << '\n';
                    body_ << "    xori " << dst << ", " << dst << ", 1\n";
                }
                return true;
            }
            return false;
        }

        bool emitImmediateBinary(const Expr& expr) {
            return emitImmediateBinaryTo(expr, "a0");
        }

        void emitLogicalAnd(const Expr& expr) {
            const std::string falseLabel = newLabel("and_false");
            const std::string endLabel = newLabel("and_end");
            if (parent_.options_.optimize) {
                emitBranchIfFalse(*expr.lhs, falseLabel);
                emitBranchIfFalse(*expr.rhs, falseLabel);
            } else {
                emitExpr(*expr.lhs);
                body_ << "    beq a0, zero, " << falseLabel << '\n';
                emitExpr(*expr.rhs);
                body_ << "    beq a0, zero, " << falseLabel << '\n';
            }
            body_ << "    li a0, 1\n";
            body_ << "    jal zero, " << endLabel << '\n';
            body_ << falseLabel << ":\n";
            body_ << "    li a0, 0\n";
            body_ << endLabel << ":\n";
        }

        void emitLogicalOr(const Expr& expr) {
            const std::string trueLabel = newLabel("or_true");
            const std::string endLabel = newLabel("or_end");
            if (parent_.options_.optimize) {
                emitBranchIfTrue(*expr.lhs, trueLabel);
                emitBranchIfTrue(*expr.rhs, trueLabel);
            } else {
                emitExpr(*expr.lhs);
                body_ << "    bne a0, zero, " << trueLabel << '\n';
                emitExpr(*expr.rhs);
                body_ << "    bne a0, zero, " << trueLabel << '\n';
            }
            body_ << "    li a0, 0\n";
            body_ << "    jal zero, " << endLabel << '\n';
            body_ << trueLabel << ":\n";
            body_ << "    li a0, 1\n";
            body_ << endLabel << ":\n";
        }

        void emitCall(const Expr& expr) {
            const auto function = parent_.functions_.find(expr.name);
            if (function == parent_.functions_.end()) {
                fail(expr.loc, "unknown function: " + expr.name);
            }

            if (parent_.options_.optimize && emitInlineCall(expr, function->second)) {
                return;
            }

            const bool directArgs = parent_.options_.optimize && canPassDirectArgs(expr);
            const bool noCallArgs = parent_.options_.optimize && !directArgs && canPassNoCallArgs(expr);
            const std::size_t regArgCount = std::min<std::size_t>(expr.args.size(), 8);
            const std::size_t stackArgCount = expr.args.size() > 8 ? expr.args.size() - 8 : 0;
            const std::size_t tempArgCount = (directArgs || noCallArgs) ? 0 : regArgCount;
            const int tempBase = static_cast<int>(stackArgCount * 4);
            const int reserve = alignTo16(static_cast<int>((stackArgCount + tempArgCount) * 4));
            if (reserve > 0) {
                emitRegAdd(body_, "sp", "sp", -reserve);
            }
            if (directArgs) {
                for (std::size_t i = 8; i < expr.args.size(); ++i) {
                    emitValueTo(*expr.args[i], "t0");
                    emitStoreOffset(body_, "t0", static_cast<int>((i - 8) * 4), "sp");
                }
                for (std::size_t i = 0; i < regArgCount; ++i) {
                    emitValueTo(*expr.args[i], argRegisterName(i));
                }
            } else if (noCallArgs) {
                for (std::size_t i = 8; i < expr.args.size(); ++i) {
                    emitExpr(*expr.args[i]);
                    emitStoreOffset(body_, "a0", static_cast<int>((i - 8) * 4), "sp");
                }
                for (std::size_t i = regArgCount; i > 0; --i) {
                    const std::size_t index = i - 1;
                    emitExpr(*expr.args[index]);
                    if (index != 0) {
                        body_ << "    addi " << argRegisterName(index) << ", a0, 0\n";
                    }
                }
            } else {
                for (std::size_t i = 0; i < expr.args.size(); ++i) {
                    emitExpr(*expr.args[i]);
                    if (i < 8) {
                        emitStoreOffset(body_, "a0", tempBase + static_cast<int>(i * 4), "sp");
                    } else {
                        emitStoreOffset(body_, "a0", static_cast<int>((i - 8) * 4), "sp");
                    }
                }
                for (std::size_t i = 0; i < regArgCount; ++i) {
                    emitLoadOffset(body_, argRegisterName(i), tempBase + static_cast<int>(i * 4), "sp");
                }
            }
            body_ << "    call " << function->second.label << '\n';
            if (reserve > 0) {
                emitRegAdd(body_, "sp", "sp", reserve);
            }
        }

        bool canPassDirectArgs(const Expr& expr) const {
            for (const auto& arg : expr.args) {
                if (!isDirectValue(*arg)) {
                    return false;
                }
            }
            return true;
        }

        bool canPassNoCallArgs(const Expr& expr) const {
            for (const auto& arg : expr.args) {
                if (exprContainsCall(*arg)) {
                    return false;
                }
            }
            return true;
        }

        bool emitInlineCall(const Expr& expr, const FunctionInfo& info) {
            const bool straightReturn = info.inlineReturn != nullptr;
            const bool branchReturn = info.inlineCondition != nullptr;
            if ((!straightReturn && !branchReturn) || info.params.size() != expr.args.size() ||
                info.function == &function_ || inlineDepth_ >= 8 ||
                (branchReturn && stmtContainsCall(*info.function->body))) {
                return false;
            }

            ++inlineDepth_;
            pushScope();
            for (std::size_t i = 0; i < expr.args.size(); ++i) {
                Symbol symbol;
                if (!bindInlineArg(*expr.args[i], symbol)) {
                    symbol = allocateLocal();
                    if (!symbol.reg.empty()) {
                        emitExprTo(*expr.args[i], symbol.reg);
                    } else {
                        emitExpr(*expr.args[i]);
                        storeSymbol(symbol, "a0");
                    }
                } else {
                    symbol.ownsLocalRegister = false;
                }
                currentScope()[info.params[i]] = std::move(symbol);
            }
            for (std::size_t i = 0; i < info.inlineDeclCount; ++i) {
                emitDecl(*info.inlineBlock->statements[i]->decl);
            }

            if (straightReturn) {
                emitExpr(*info.inlineReturn);
            } else if (const auto condition = evalConst(*info.inlineCondition)) {
                emitExpr(*(*condition != 0 ? info.inlineThenReturn : info.inlineElseReturn));
            } else {
                const std::string elseLabel = newLabel("inline_else");
                const std::string endLabel = newLabel("inline_end");
                emitBranchIfFalse(*info.inlineCondition, elseLabel);
                emitExpr(*info.inlineThenReturn);
                body_ << "    jal zero, " << endLabel << '\n';
                body_ << elseLabel << ":\n";
                emitExpr(*info.inlineElseReturn);
                body_ << endLabel << ":\n";
            }
            popScope();
            --inlineDepth_;
            return true;
        }

        bool bindInlineArg(const Expr& arg, Symbol& symbol) {
            if (arg.kind == ExprKind::Number) {
                symbol = Symbol{SymbolKind::Const, 0, "", toInt32(arg.number), ""};
                return true;
            }
            if (arg.kind != ExprKind::Variable) {
                return false;
            }
            const auto value = lookup(arg.name);
            if (!value || value->kind == SymbolKind::GlobalVar) {
                return false;
            }
            symbol = *value;
            return true;
        }

        static std::uint64_t loopUseWeight(std::uint64_t weight) {
            constexpr std::uint64_t kMaxWeight =
                std::numeric_limits<std::uint64_t>::max() / 32;
            return weight > kMaxWeight ? std::numeric_limits<std::uint64_t>::max()
                                       : weight * 32;
        }

        void collectExprUses(const Expr& expr, std::uint64_t weight) {
            switch (expr.kind) {
                case ExprKind::Number:
                    return;
                case ExprKind::Variable:
                    referencedNames_.insert(expr.name);
                    variableUseWeights_[expr.name] += weight;
                    return;
                case ExprKind::Unary:
                    collectExprUses(*expr.lhs, weight);
                    return;
                case ExprKind::Binary:
                    collectExprUses(*expr.lhs, weight);
                    collectExprUses(*expr.rhs, weight);
                    return;
                case ExprKind::Call:
                    for (const auto& arg : expr.args) {
                        collectExprUses(*arg, weight);
                    }
                    return;
            }
        }

        void collectVariableUses(const Stmt& stmt, std::uint64_t weight) {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        collectVariableUses(*child, weight);
                    }
                    return;
                case StmtKind::Empty:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return;
                case StmtKind::ExprStmt:
                case StmtKind::Return:
                    if (stmt.expr) {
                        collectExprUses(*stmt.expr, weight);
                    }
                    return;
                case StmtKind::Assign:
                    assignedNames_.insert(stmt.name);
                    if (stmt.expr) {
                        collectExprUses(*stmt.expr, weight);
                    }
                    return;
                case StmtKind::DeclStmt:
                    if (stmt.decl) {
                        localCandidateNames_.insert(stmt.decl->name);
                    }
                    if (stmt.decl && stmt.decl->init) {
                        collectExprUses(*stmt.decl->init, weight);
                    }
                    return;
                case StmtKind::If:
                    collectExprUses(*stmt.expr, weight);
                    collectVariableUses(*stmt.thenBranch, weight);
                    if (stmt.elseBranch) {
                        collectVariableUses(*stmt.elseBranch, weight);
                    }
                    return;
                case StmtKind::While: {
                    const std::uint64_t loopWeight = loopUseWeight(weight);
                    collectExprUses(*stmt.expr, loopWeight);
                    collectVariableUses(*stmt.thenBranch, loopWeight);
                    return;
                }
            }
        }

        bool stmtAlwaysTerminates(const Stmt& stmt) const {
            switch (stmt.kind) {
                case StmtKind::Return:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return true;
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        if (stmtAlwaysTerminates(*child)) {
                            return true;
                        }
                    }
                    return false;
                case StmtKind::If:
                    return stmt.elseBranch && stmtAlwaysTerminates(*stmt.thenBranch) &&
                           stmtAlwaysTerminates(*stmt.elseBranch);
                case StmtKind::Empty:
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                case StmtKind::DeclStmt:
                case StmtKind::While:
                    return false;
            }
            return false;
        }

        bool declContainsCall(const Decl& decl) const {
            return decl.init && exprContainsCall(*decl.init);
        }

        bool stmtContainsCall(const Stmt& stmt) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        if (stmtContainsCall(*child)) {
                            return true;
                        }
                    }
                    return false;
                case StmtKind::Empty:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return false;
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                case StmtKind::Return:
                    return stmt.expr && exprContainsCall(*stmt.expr);
                case StmtKind::DeclStmt:
                    return stmt.decl && declContainsCall(*stmt.decl);
                case StmtKind::If:
                    return (stmt.expr && exprContainsCall(*stmt.expr)) ||
                           (stmt.thenBranch && stmtContainsCall(*stmt.thenBranch)) ||
                           (stmt.elseBranch && stmtContainsCall(*stmt.elseBranch));
                case StmtKind::While:
                    return (stmt.expr && exprContainsCall(*stmt.expr)) ||
                           (stmt.thenBranch && stmtContainsCall(*stmt.thenBranch));
            }
            return false;
        }

        bool callIsFullyInlineable(const Expr& expr) const {
            const auto function = parent_.functions_.find(expr.name);
            if (function == parent_.functions_.end() ||
                (!function->second.inlineReturn && !function->second.inlineCondition) ||
                function->second.function == &function_) {
                return false;
            }
            return function->second.function && !stmtContainsCall(*function->second.function->body);
        }

        bool isTailRecursiveCallExpr(const Expr& expr) const {
            return expr.kind == ExprKind::Call && expr.name == function_.name &&
                   expr.args.size() == function_.params.size();
        }

        bool stmtContainsRuntimeCall(const Stmt& stmt) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        if (stmtContainsRuntimeCall(*child)) {
                            return true;
                        }
                    }
                    return false;
                case StmtKind::Empty:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return false;
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                    return stmt.expr && exprContainsRuntimeCall(*stmt.expr);
                case StmtKind::Return:
                    if (!stmt.expr) {
                        return false;
                    }
                    if (isTailRecursiveCallExpr(*stmt.expr)) {
                        for (const auto& arg : stmt.expr->args) {
                            if (exprContainsRuntimeCall(*arg)) {
                                return true;
                            }
                        }
                        return false;
                    }
                    return exprContainsRuntimeCall(*stmt.expr);
                case StmtKind::DeclStmt:
                    return stmt.decl && stmt.decl->init && exprContainsRuntimeCall(*stmt.decl->init);
                case StmtKind::If:
                    return (stmt.expr && exprContainsRuntimeCall(*stmt.expr)) ||
                           (stmt.thenBranch && stmtContainsRuntimeCall(*stmt.thenBranch)) ||
                           (stmt.elseBranch && stmtContainsRuntimeCall(*stmt.elseBranch));
                case StmtKind::While:
                    return (stmt.expr && exprContainsRuntimeCall(*stmt.expr)) ||
                           (stmt.thenBranch && stmtContainsRuntimeCall(*stmt.thenBranch));
            }
            return false;
        }

        bool exprContainsRuntimeCall(const Expr& expr) const {
            switch (expr.kind) {
                case ExprKind::Number:
                case ExprKind::Variable:
                    return false;
                case ExprKind::Unary:
                    return exprContainsRuntimeCall(*expr.lhs);
                case ExprKind::Binary:
                    return exprContainsRuntimeCall(*expr.lhs) || exprContainsRuntimeCall(*expr.rhs);
                case ExprKind::Call:
                    for (const auto& arg : expr.args) {
                        if (exprContainsRuntimeCall(*arg)) {
                            return true;
                        }
                    }
                    return !callIsFullyInlineable(expr);
            }
            return false;
        }

        bool exprContainsCall(const Expr& expr) const {
            switch (expr.kind) {
                case ExprKind::Number:
                case ExprKind::Variable:
                    return false;
                case ExprKind::Unary:
                    return exprContainsCall(*expr.lhs);
                case ExprKind::Binary:
                    return exprContainsCall(*expr.lhs) || exprContainsCall(*expr.rhs);
                case ExprKind::Call:
                    return true;
            }
            return false;
        }

        std::string tempRegisterName(int index) const {
            static const std::vector<std::string> regs = {"t1", "t2", "t3", "t4"};
            return regs.at(static_cast<std::size_t>(index));
        }

        int tempRegisterCount() const {
            return 4;
        }

        void pushA0(bool forceStack = false) {
            if (parent_.options_.optimize && !forceStack && tempRegDepth_ < tempRegisterCount()) {
                body_ << "    addi " << tempRegisterName(tempRegDepth_) << ", a0, 0\n";
                ++tempRegDepth_;
                tempLocationIsReg_.push_back(true);
                return;
            }
            body_ << "    addi sp, sp, -16\n";
            body_ << "    sw a0, 12(sp)\n";
            ++tempStackDepth_;
            tempLocationIsReg_.push_back(false);
        }

        void popTo(const std::string& reg) {
            const std::string valueReg = popValue(reg);
            if (valueReg != reg) {
                body_ << "    addi " << reg << ", " << valueReg << ", 0\n";
            }
        }

        std::string popValue(const std::string& fallbackReg) {
            if (tempLocationIsReg_.empty()) {
                throw std::runtime_error("internal error: expression stack underflow");
            }
            const bool useReg = tempLocationIsReg_.back();
            tempLocationIsReg_.pop_back();
            if (useReg) {
                --tempRegDepth_;
                return tempRegisterName(tempRegDepth_);
            }
            if (tempStackDepth_ <= 0) {
                throw std::runtime_error("internal error: expression stack underflow");
            }
            --tempStackDepth_;
            body_ << "    lw " << fallbackReg << ", 12(sp)\n";
            body_ << "    addi sp, sp, 16\n";
            return fallbackReg;
        }
    };

    void collectFunctions() {
        for (const auto& function : unit_.functions) {
            FunctionInfo info;
            info.returnType = function->returnType;
            info.params = function->params;
            info.label = functionLabel(function->name);
            info.function = function.get();
            info.inlineReturn = simpleInlineReturnExpr(*function);
            if (info.inlineReturn) {
                info.inlineBlock = function->body.get();
                info.inlineDeclCount = function->body->statements.size() - 1;
            } else if (const auto branch = simpleInlineBranch(*function)) {
                info.inlineBlock = function->body.get();
                info.inlineDeclCount = branch->declCount;
                info.inlineCondition = branch->condition;
                info.inlineThenReturn = branch->thenReturn;
                info.inlineElseReturn = branch->elseReturn;
            }
            functions_[function->name] = std::move(info);
        }
    }

    std::optional<Symbol> lookupGlobal(const std::string& name) const {
        const auto it = globals_.find(name);
        if (it == globals_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::optional<std::int32_t> evalGlobalConst(const Expr& expr) const {
        switch (expr.kind) {
            case ExprKind::Number:
                return toInt32(expr.number);
            case ExprKind::Variable: {
                const auto symbol = lookupGlobal(expr.name);
                if (symbol && symbol->kind == SymbolKind::Const) {
                    return symbol->constValue;
                }
                return std::nullopt;
            }
            case ExprKind::Unary: {
                const auto value = evalGlobalConst(*expr.lhs);
                if (!value) {
                    return std::nullopt;
                }
                if (expr.op == "+") {
                    return *value;
                }
                if (expr.op == "-") {
                    return toInt32(-static_cast<std::int64_t>(*value));
                }
                if (expr.op == "!") {
                    return static_cast<std::int32_t>(*value == 0 ? 1 : 0);
                }
                return std::nullopt;
            }
            case ExprKind::Binary:
                return evalGlobalBinary(expr);
            case ExprKind::Call:
                return std::nullopt;
        }
        return std::nullopt;
    }

    std::optional<std::int32_t> evalGlobalBinary(const Expr& expr) const {
        if (expr.op == "&&") {
            const auto lhs = evalGlobalConst(*expr.lhs);
            if (!lhs) {
                return std::nullopt;
            }
            if (*lhs == 0) {
                return 0;
            }
            const auto rhs = evalGlobalConst(*expr.rhs);
            if (!rhs) {
                return std::nullopt;
            }
            return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
        }
        if (expr.op == "||") {
            const auto lhs = evalGlobalConst(*expr.lhs);
            if (!lhs) {
                return std::nullopt;
            }
            if (*lhs != 0) {
                return 1;
            }
            const auto rhs = evalGlobalConst(*expr.rhs);
            if (!rhs) {
                return std::nullopt;
            }
            return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
        }

        const auto lhs = evalGlobalConst(*expr.lhs);
        const auto rhs = evalGlobalConst(*expr.rhs);
        if (!lhs || !rhs) {
            return std::nullopt;
        }
        const std::int64_t left = *lhs;
        const std::int64_t right = *rhs;
        if (expr.op == "+") {
            return toInt32(left + right);
        }
        if (expr.op == "-") {
            return toInt32(left - right);
        }
        if (expr.op == "*") {
            return toInt32(left * right);
        }
        if (expr.op == "/") {
            if (right == 0) {
                return std::nullopt;
            }
            return toInt32(left / right);
        }
        if (expr.op == "%") {
            if (right == 0) {
                return std::nullopt;
            }
            return toInt32(left % right);
        }
        if (expr.op == "<") {
            return static_cast<std::int32_t>(left < right ? 1 : 0);
        }
        if (expr.op == ">") {
            return static_cast<std::int32_t>(left > right ? 1 : 0);
        }
        if (expr.op == "<=") {
            return static_cast<std::int32_t>(left <= right ? 1 : 0);
        }
        if (expr.op == ">=") {
            return static_cast<std::int32_t>(left >= right ? 1 : 0);
        }
        if (expr.op == "==") {
            return static_cast<std::int32_t>(left == right ? 1 : 0);
        }
        if (expr.op == "!=") {
            return static_cast<std::int32_t>(left != right ? 1 : 0);
        }
        return std::nullopt;
    }

    void collectGlobals() {
        for (const auto& decl : unit_.globals) {
            const auto value = evalGlobalConst(*decl->init);
            if (!value) {
                fail(decl->loc, "global initializer is not a compile-time value");
            }

            if (decl->isConst) {
                globals_[decl->name] = Symbol{SymbolKind::Const, 0, "", *value, ""};
            } else {
                const std::string label = globalLabel(decl->name);
                globals_[decl->name] = Symbol{SymbolKind::GlobalVar, 0, label, 0, ""};
                std::ostringstream line;
                line << label << ":\n";
                line << "    .word " << printableI32(*value) << '\n';
                dataLines_.push_back(line.str());
            }
        }
    }

    std::string generateFunction(const Function& function) {
        FunctionEmitter emitter(*this, function);
        return emitter.emit();
    }
};


} // namespace

std::string generateRiscV(const CompUnit& unit, Options options) {
    CodeGenerator generator(unit, options);
    return generator.generate();
}

} // namespace toyc
