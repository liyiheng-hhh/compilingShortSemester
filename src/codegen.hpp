#pragma once

#include "ast.hpp"
#include "common.hpp"

#include <string>

namespace toyc {

std::string generateRiscV(const CompUnit& unit, Options options);

} // namespace toyc
