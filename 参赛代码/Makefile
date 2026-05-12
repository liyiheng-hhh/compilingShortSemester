CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

all: compiler

compiler: compiler.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f compiler

check: compiler
	./compiler -S -o examples/smoke.s examples/smoke.sy
	./compiler -S -o examples/smoke_O1.s examples/smoke.sy -O1

.PHONY: all clean check
