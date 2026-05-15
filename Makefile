CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

# 供 compile-all / size-report 递归扫描（空格分隔多个根目录）
SY_DIRS ?= examples

SRCDIR := src
SRCS := $(SRCDIR)/main.cpp $(SRCDIR)/common.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/parser.cpp \
	$(SRCDIR)/semantic.cpp $(SRCDIR)/codegen.cpp $(SRCDIR)/ir_build.cpp $(SRCDIR)/ir_opt.cpp
OBJS := $(SRCS:.cpp=.o)

all: compiler

compiler: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f compiler $(OBJS)

check: compiler
	./compiler -S -o examples/smoke.s examples/smoke.sy
	./compiler -S -o examples/smoke_O1.s examples/smoke.sy -O1
	./compiler -S -O1 -o examples/_cmp_check.s examples/golden_o1_cmp/cmp.sy
	./compiler -S -O1 -o examples/_magic_check.s examples/golden_magic_div/boundary.sy
	rm -f examples/_cmp_check.s examples/_magic_check.s

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

# 多目录批量（空格分隔，传给 e2e-docker.sh 的 SY_TEST_DIRS）
#   make docker-test-dirs SY_TEST_DIRS="/work/examples /work/2026初赛RISCV赛道功能用例/functional"
docker-test-dirs:
	@if [ -z "$(SY_TEST_DIRS)" ]; then echo 'docker-test-dirs: set SY_TEST_DIRS="dir1 dir2"'; exit 1; fi
	@SY_TEST_DIRS="$(SY_TEST_DIRS)" ./scripts/docker-test-container.sh test ""

.PHONY: all clean check sytest sytest-o0 sytest-o1 compile-all compile-all-o1 size-report perf-profile \
	local-eval docker-local-eval docker-init docker-test-functional docker-test-performance docker-test-dirs
