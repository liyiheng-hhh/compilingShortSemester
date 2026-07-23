#pragma once

#include "ast.hpp"
#include "common.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace toyc {

// Evaluate a closed ToyC program at compile time when possible.
std::optional<std::int32_t> evaluateWholeProgram(CompUnit& unit);
std::string emitConstantProgram(std::int32_t value);

} // namespace toyc
