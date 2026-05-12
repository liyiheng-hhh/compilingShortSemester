CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

SRCS = main.cpp common.cpp lexer.cpp parser.cpp semantic.cpp codegen.cpp
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

.PHONY: all clean check
