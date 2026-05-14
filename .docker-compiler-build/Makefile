CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

SRCS = main.cpp common.cpp lexer.cpp parser.cpp semantic.cpp codegen.cpp ir_build.cpp ir_opt.cpp
OBJS = $(SRCS:.cpp=.o)

all: compiler

compiler: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f compiler $(OBJS)

check: compiler
	./compiler -S -o examples/smoke.s examples/smoke.sy
	./compiler -S -o examples/smoke_O1.s examples/smoke.sy -O1
	./compiler -S -O1 -o examples/_cmp_check.s examples/golden_o1_cmp/cmp.sy
	rm -f examples/_cmp_check.s

# 批量功能回归（需本机已安装 riscv64-linux-gnu-gcc、qemu-riscv64-static）
#   make sytest LIBSYSY=/path/to/libsysy.a TESTDIR=examples/golden_smoke
sytest: compiler
	@if [ -z "$(LIBSYSY)" ]; then echo 'sytest: set LIBSYSY=/path/to/libsysy.a'; exit 1; fi
	@if [ -z "$(TESTDIR)" ]; then echo 'sytest: set TESTDIR=directory-with-.sy'; exit 1; fi
	LIBSYSY="$(LIBSYSY)" ./scripts/run_sy_tests.sh "$(TESTDIR)"

.PHONY: all clean check sytest
