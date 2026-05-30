CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
# 可选：`CXXFLAGS_EXTRA=-DSYSY_O1_FULL=1 make` → 默认档 D；未设时 `-O1` 为档 A（Codegen）见 src/opt_config.h

# 供 compile-all / size-report 递归扫描（空格分隔多个根目录）
SY_DIRS ?= examples

SRCDIR := src
SRCS := $(SRCDIR)/main.cpp $(SRCDIR)/common.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/lexer_io.cpp \
	$(SRCDIR)/lexer_scan.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/parser_util.cpp \
	$(SRCDIR)/parser_expr.cpp \
	$(SRCDIR)/semantic.cpp $(SRCDIR)/semantic_const.cpp $(SRCDIR)/semantic_visit.cpp \
	$(SRCDIR)/codegen.cpp $(SRCDIR)/ir_build.cpp $(SRCDIR)/ir_opt.cpp \
	$(SRCDIR)/ir_loop_opt.cpp $(SRCDIR)/ir_regalloc.cpp $(SRCDIR)/ir_mem2reg.cpp $(SRCDIR)/ir_expr_gvn.cpp \
	$(SRCDIR)/ir_schedule.cpp \
	$(SRCDIR)/loop_interchange.cpp $(SRCDIR)/loop_tiling.cpp $(SRCDIR)/land_lor_split.cpp \
	$(SRCDIR)/knapsack_dp.cpp $(SRCDIR)/mm_hoist.cpp $(SRCDIR)/loop_unroll.cpp $(SRCDIR)/row_scratch_matmul.cpp \
	$(SRCDIR)/rv/rv_asm.cpp $(SRCDIR)/rv/rv_passes.cpp $(SRCDIR)/rv/BranchPeephole.cpp \
	$(SRCDIR)/rv/RegPeephole.cpp \
	$(SRCDIR)/rv/InstCombine.cpp $(SRCDIR)/rv/Schedule.cpp $(SRCDIR)/rv/StrengthReduct.cpp \
	$(SRCDIR)/codegen/OpBase.cpp $(SRCDIR)/codegen/OpBaseDom.cpp $(SRCDIR)/codegen/Attrs.cpp \
	$(SRCDIR)/codegen/CodeGen.cpp $(SRCDIR)/codegen/CodeGenBuilder.cpp \
	$(SRCDIR)/codegen/CodeGenEmitExpr.cpp $(SRCDIR)/codegen/CodeGenEmitStmt.cpp \
	$(SRCDIR)/pre-opt/PreAttrs.cpp $(SRCDIR)/pre-opt/MoveAlloca.cpp \
	$(SRCDIR)/pre-opt/EarlyConstFold.cpp $(SRCDIR)/pre-opt/Localize.cpp \
	$(SRCDIR)/pre-opt/EarlyInline.cpp $(SRCDIR)/pre-opt/TCO.cpp \
	$(SRCDIR)/pre-opt/LoweredTCO.cpp $(SRCDIR)/pre-opt/Remerge.cpp \
	$(SRCDIR)/pre-opt/RaiseToFor.cpp $(SRCDIR)/pre-opt/View.cpp \
	$(SRCDIR)/pre-opt/LoopDCE.cpp $(SRCDIR)/pre-opt/TidyMemory.cpp \
	$(SRCDIR)/pre-opt/Lower.cpp $(SRCDIR)/pre-opt/ArrayAccess.cpp \
	$(SRCDIR)/pre-opt/Base.cpp $(SRCDIR)/pre-opt/ColumnMajor.cpp \
	$(SRCDIR)/pre-opt/Fusion.cpp $(SRCDIR)/pre-opt/NoStore.cpp \
	$(SRCDIR)/pre-opt/Parallelizable.cpp \
	$(SRCDIR)/opt/Pass.cpp $(SRCDIR)/opt/GVN.cpp \
	$(SRCDIR)/mlir_rv/Lower.cpp $(SRCDIR)/mlir_rv/InstCombine.cpp $(SRCDIR)/mlir_rv/RvDCE.cpp \
	$(SRCDIR)/mlir_rv/StrengthReduct.cpp $(SRCDIR)/mlir_rv/Schedule.cpp $(SRCDIR)/mlir_rv/RegAlloc.cpp $(SRCDIR)/mlir_rv/RegPeephole.cpp \
	$(SRCDIR)/mlir_rv/Dump.cpp \
	$(SRCDIR)/rv_mlir_pipeline.cpp \
	$(SRCDIR)/dialect_pipeline.cpp $(SRCDIR)/dialect_fallback.cpp \
	$(SRCDIR)/dialect_parse/Lexer.cpp $(SRCDIR)/dialect_parse/LexerKeywords.cpp \
	$(SRCDIR)/dialect_parse/LexerNext.cpp 	$(SRCDIR)/dialect_parse/ParserConst.cpp \
	$(SRCDIR)/dialect_parse/ParserToken.cpp $(SRCDIR)/dialect_parse/ParserType.cpp \
	$(SRCDIR)/dialect_parse/ParserExpr.cpp $(SRCDIR)/dialect_parse/ParserDecl.cpp \
	$(SRCDIR)/dialect_parse/ParserFold.cpp $(SRCDIR)/dialect_parse/ParserDriver.cpp \
	$(SRCDIR)/dialect_parse/SemaTypes.cpp $(SRCDIR)/dialect_parse/SemaInfer.cpp \
	$(SRCDIR)/dialect_parse/Sema.cpp $(SRCDIR)/dialect_parse/KnapsackDp.cpp \
	$(SRCDIR)/dialect_parse/Type.cpp \
	$(SRCDIR)/opt/PassManager.cpp $(SRCDIR)/opt/Mem2Reg.cpp $(SRCDIR)/opt/BitStubFold.cpp \
	$(SRCDIR)/opt/Alias.cpp $(SRCDIR)/opt/Pureness.cpp $(SRCDIR)/opt/AtMostOnce.cpp \
	$(SRCDIR)/opt/DSE.cpp $(SRCDIR)/opt/DLE.cpp $(SRCDIR)/opt/DAE.cpp \
	$(SRCDIR)/opt/AggressiveDCE.cpp \
	$(SRCDIR)/opt/LoopInfo.cpp $(SRCDIR)/opt/LoopAnalysis.cpp \
	$(SRCDIR)/opt/CanonicalizeLoop.cpp $(SRCDIR)/opt/LoopRotate.cpp \
	$(SRCDIR)/opt/LoopUnroll.cpp $(SRCDIR)/opt/HoistLoopGlobal.cpp $(SRCDIR)/opt/LICM.cpp $(SRCDIR)/opt/SCEV.cpp \
	$(SRCDIR)/opt/RowScratchMatmul.cpp $(SRCDIR)/opt/LoopTiling.cpp \
	$(SRCDIR)/opt/RemoveEmptyLoop.cpp $(SRCDIR)/opt/Inline.cpp \
	$(SRCDIR)/opt/FlattenCFG.cpp \
	$(SRCDIR)/opt/RegularFold.cpp \
	$(SRCDIR)/opt/MatKernelOpt.cpp \
	$(SRCDIR)/opt/GCM.cpp $(SRCDIR)/opt/Select.cpp $(SRCDIR)/opt/InstSchedule.cpp \
	$(SRCDIR)/opt/LateInline.cpp $(SRCDIR)/opt/Reassociate.cpp \
	$(SRCDIR)/opt/ArrayStrideAnalysis.cpp $(SRCDIR)/opt/Globalize.cpp \
	$(SRCDIR)/opt/InlineStore.cpp $(SRCDIR)/opt/SynthConstArray.cpp \
	$(SRCDIR)/opt/DCE.cpp $(SRCDIR)/opt/SimplifyCFG.cpp $(SRCDIR)/opt/CallGraph.cpp \
	$(SRCDIR)/opt/Verify.cpp \
	$(SRCDIR)/utils/Exec.cpp \
	$(SRCDIR)/utils/MatcherCore.cpp $(SRCDIR)/utils/MatcherMatch.cpp \
	$(SRCDIR)/utils/MatcherEval.cpp $(SRCDIR)/utils/MatcherBuild.cpp \
	$(SRCDIR)/utils/smt/BvMatcher.cpp $(SRCDIR)/utils/smt/CDCL.cpp \
	$(SRCDIR)/utils/smt/CDCLShared.cpp $(SRCDIR)/utils/smt/CDCLPropagate.cpp \
	$(SRCDIR)/utils/smt/CDCLAnalyze.cpp \
	$(SRCDIR)/utils/smt/Simplify.cpp $(SRCDIR)/utils/smt/Solve.cpp $(SRCDIR)/utils/smt/SolvePropagate.cpp \
	$(SRCDIR)/main/Options.cpp
OBJS := $(SRCS:.cpp=.o)

all: compiler

compiler: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -o $@ $(OBJS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_EXTRA) -c $< -o $@

clean:
	rm -f compiler $(OBJS)

check: compiler
	./compiler -S -o examples/smoke.s examples/smoke.sy
	./compiler -S -o examples/smoke_O1.s examples/smoke.sy -O1
	./compiler -S -O1 -o examples/_cmp_check.s examples/golden_o1_cmp/cmp.sy
	./compiler -S -O1 -o examples/_magic_check.s examples/golden_magic_div/boundary.sy
	./compiler -S -O1 -o examples/_shift_check.s examples/golden_shift/shift.sy
	./compiler -S -O1 -o examples/_loop_ix_check.s examples/golden_transpose_ix/transpose_ix.sy
	./compiler -S -O1 -o examples/_loop_ix_tri_check.s examples/golden_transpose_ix/transpose_ix_tri_guard.sy
	./compiler -S -O1 -o examples/_tile_kouter_check.s examples/golden_tile_kouter/kouter_assign.sy
	./compiler -S -O1 -o examples/_mmc_mini_check.s examples/golden_many_mat_cal_mini/many_mat_cal_mini.sy
	./compiler -S -O1 -o examples/_h10_check.s performance/h-10-01.sy
	./compiler -S -O1 -o examples/_conv2d_check.s performance/conv2d-1.sy
	./compiler -S -O1 -o examples/_land_split_check.s examples/golden_land_split/while_and.sy
	./compiler -S -O1 -o examples/_huffman_check.s performance/huffman-01.sy
	./compiler -S -O1 -o examples/_knapsack_check.s performance/knapsack_naive-1.sy
	./compiler -S -O1 -o examples/_knapsack_mini_check.s examples/golden_knapsack/knapsack_mini.sy
	./compiler -S -o examples/_shift_check0.s examples/golden_shift/shift.sy
	rm -f examples/_cmp_check.s examples/_magic_check.s examples/_shift_check.s examples/_shift_check0.s examples/_loop_ix_check.s examples/_loop_ix_tri_check.s examples/_tile_kouter_check.s examples/_mmc_mini_check.s examples/_h10_check.s examples/_conv2d_check.s examples/_land_split_check.s examples/_huffman_check.s examples/_knapsack_check.s examples/_knapsack_mini_check.s

# 递归编译所有 .sy → .s（无需 qemu；大测试树传 SY_DIRS）
#   make compile-all SY_DIRS="examples path/to/functional path/to/performance"
#   make compile-all-o1 SY_DIRS="path/to/performance"
compile-all: compiler
	@./scripts/compile_all_sy.sh "$(SY_DIRS)"

compile-all-o1: compiler
	@OLEVEL=-O1 ./scripts/compile_all_sy.sh "$(SY_DIRS)"

# 批量功能回归（需本机已安装 riscv64-linux-gnu-gcc、qemu-riscv64-static）
#   make sytest LIBSYSY=/path/to/libsysy.a TESTDIR=examples/golden_smoke
sytest: compiler
	@if [ -z "$(LIBSYSY)" ]; then echo 'sytest: set LIBSYSY=/path/to/libsysy.a'; exit 1; fi
	@if [ -z "$(TESTDIR)" ]; then echo 'sytest: set TESTDIR=directory-with-.sy'; exit 1; fi
	LIBSYSY="$(LIBSYSY)" ./scripts/run_sy_tests.sh "$(TESTDIR)"

sytest-o0: compiler
	@if [ -z "$(LIBSYSY)" ]; then echo 'sytest-o0: set LIBSYSY=/path/to/libsysy.a'; exit 1; fi
	@if [ -z "$(TESTDIR)" ]; then echo 'sytest-o0: set TESTDIR=directory-with-.sy'; exit 1; fi
	USE_O1=0 LIBSYSY="$(LIBSYSY)" ./scripts/run_sy_tests.sh "$(TESTDIR)"

sytest-o1: compiler
	@if [ -z "$(LIBSYSY)" ]; then echo 'sytest-o1: set LIBSYSY=/path/to/libsysy.a'; exit 1; fi
	@if [ -z "$(TESTDIR)" ]; then echo 'sytest-o1: set TESTDIR=directory-with-.sy'; exit 1; fi
	USE_O1=1 LIBSYSY="$(LIBSYSY)" ./scripts/run_sy_tests.sh "$(TESTDIR)"

# 本地评测套：local_eval_cases（小用例 + 除法/取模属性；O0/O1 批测 + O0 vs O1 对拍）
#   export LIBSYSY=/path/to/libsysy.a && make local-eval
# Docker 内先在挂载目录（如 /work）下 make，使 ./compiler 与容器 glibc 一致；或 export COMPILER=BUILD_DIR/compiler
local-eval:
	@if [ -z "$(LIBSYSY)" ]; then echo 'local-eval: set LIBSYSY=/path/to/libsysy.a'; exit 1; fi
	@LIBSYSY="$(LIBSYSY)" ./scripts/run_local_eval.sh

# 单条性能用例 Profiling（见 scripts/profile_sy.sh）
#   make perf-profile PERF_SY=performance/matmul1.sy
#   make perf-profile PERF_SY=performance/fft0.sy LIBSYSY=/path/to/libsysy.a
PERF_SY ?= examples/smoke.sy
perf-profile: compiler
	@bash ./scripts/profile_sy.sh "$(PERF_SY)"

# ---- 运行时性能评测（eval-runtime 工作流）----
#   make libsysy.a
#   RUNTIME_PERF_TIMEOUT_SEC=120 make runtime-eval SUITE=performance OPT=O1
#   make runtime-summary
#   make runtime-compare SUITE=performance
#   make runtime-gate PERF_TIMEOUT=20
#   make docker-runtime-perf
#   make docker-runtime-gate
# 支持直接链接 runtime .c 或预编译 libsysy.a
RUNTIME_C ?= runtime/sysy_runtime.c
RUNTIME_O ?= runtime/sysy_runtime.o

libsysy.a: $(RUNTIME_C)
	riscv64-linux-gnu-gcc -Wall -O3 -c -o $(RUNTIME_O) $(RUNTIME_C)
	riscv64-linux-gnu-ar rcs $@ $(RUNTIME_O)

# 直接链接 runtime .c（避免部分环境下静态库兼容问题）
link-runtime-c: $(RUNTIME_C)
	@echo "$(RUNTIME_C)"
	riscv64-linux-gnu-ranlib $@

SUITE ?= performance
OPT ?= O1
RUNTIME_ROOT_DIR ?= tests/.out/runtime
PERF_TIMEOUT ?= 20
RUNTIME_CSV ?=

runtime-eval: compiler libsysy.a
	@chmod +x scripts/eval-runtime.sh scripts/eval_perf.sh scripts/runtime-summary.sh 2>/dev/null || true
	@LIBSYSY="$(CURDIR)/libsysy.a" \
	  RUNTIME_CSV="$(RUNTIME_CSV)" RUNTIME_ROOT="$(RUNTIME_ROOT_DIR)" \
	  ./scripts/eval-runtime.sh "$(SUITE)" "$(OPT)"

runtime-summary:
	@chmod +x scripts/runtime-summary.sh 2>/dev/null || true
	@./scripts/runtime-summary.sh "$(if $(RUNTIME_CSV),$(RUNTIME_CSV),tests/.out/runtime/sysy-performance-O1.csv)"

runtime-compare: compiler libsysy.a
	@chmod +x scripts/eval-compare-opt.sh 2>/dev/null || true
	@LIBSYSY="$(CURDIR)/libsysy.a" ./scripts/eval-compare-opt.sh "$(SUITE)" "$(PERF_TIMEOUT)"

runtime-hotspots: compiler libsysy.a
	@chmod +x scripts/eval-hotspots.sh 2>/dev/null || true
	@LIBSYSY="$(CURDIR)/libsysy.a" ./scripts/eval-hotspots.sh "$(OPT)" "$(PERF_TIMEOUT)"

runtime-gate: compiler libsysy.a
	@chmod +x scripts/eval-gate.sh 2>/dev/null || true
	@LIBSYSY="$(CURDIR)/libsysy.a" ./scripts/eval-gate.sh "$(PERF_TIMEOUT)"

# 兼容旧名
perf-eval: runtime-eval
perf-summary: runtime-summary

PERF_DIR ?= performance
PERF_CSV ?=

# O0 vs O1 汇编行数对比（需大测试树时设置 SY_DIRS）
size-report: compiler
	@./scripts/size_report_sy.sh "$(SY_DIRS)"

# ---- Docker 回归（宿主机需 Docker；仓库挂载为容器内 /work）----
# 官方用例目录名见 .gitignore；请放在仓库根下或自行 -v 挂载后改路径。
DOCKER_FUNC ?= /work/2026初赛RISCV赛道功能用例/functional
DOCKER_PERF ?= /work/performance

docker-init:
	@./scripts/docker-test-container.sh init

# 容器内 local_eval_cases（自动编 libsysy.a；需先 docker-init）
docker-local-eval:
	@./scripts/docker_local_eval.sh

# 功能集 O0（可覆盖 USE_O1）
docker-test-functional:
	@USE_O1=$${USE_O1:-0} ./scripts/docker-test-container.sh test "$(DOCKER_FUNC)"

# 性能集 O1（默认 USE_O1=1，可 USE_O1=0 make docker-test-performance）
docker-test-performance:
	@USE_O1=$${USE_O1:-1} ./scripts/docker-test-container.sh test "$(DOCKER_PERF)"

docker-runtime-perf:
	@./scripts/docker-test-container.sh perf performance O1

docker-runtime-gate:
	@./scripts/docker-test-container.sh gate $(PERF_TIMEOUT)

# 多目录批量（空格分隔，传给 e2e-docker.sh 的 SY_TEST_DIRS）
#   make docker-test-dirs SY_TEST_DIRS="/work/examples /work/2026初赛RISCV赛道功能用例/functional"
docker-test-dirs:
	@if [ -z "$(SY_TEST_DIRS)" ]; then echo 'docker-test-dirs: set SY_TEST_DIRS="dir1 dir2"'; exit 1; fi
	@SY_TEST_DIRS="$(SY_TEST_DIRS)" ./scripts/docker-test-container.sh test ""

.PHONY: all clean check sytest sytest-o0 sytest-o1 compile-all compile-all-o1 size-report perf-profile \
	libsysy.a runtime-eval runtime-summary runtime-compare runtime-hotspots runtime-gate \
	perf-eval perf-summary \
	local-eval docker-local-eval docker-init docker-test-functional docker-test-performance \
	docker-runtime-perf docker-runtime-gate docker-test-dirs
