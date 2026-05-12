#pragma once

// IR / SSA middle-end placeholder.
//
// Planned pipeline: AST → IR generation → optional optimization (-O1) → instruction
// selection → existing CodeGen (or lowered ASM). Extend compileFile() in main.cpp
// once modules exist.
