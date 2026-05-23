#pragma once

#include <string>

namespace sys {

// 方言 -O1 默认开启，但部分用例依赖 legacy AST/Codegen 优化；返回 true 时走 legacy O1。
bool dialectPreferLegacyPipeline(const std::string &source,
                                 const std::string &inputPath);

}  // namespace sys
