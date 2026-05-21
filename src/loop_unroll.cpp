#include "loop_unroll.h"

#include <unordered_set>

using namespace std;

// AST 级循环展开：简化版，目前只做遍历分析
// 实际展开需要完整的 AST 深拷贝支持

// 外部接口
void applySmallLoopUnrollPass(Program &program) {
  // 简化版：暂不实现实际展开
  // 避免复杂的类型转换问题

  for (auto &item : program.items) {
    if (item.func && item.func->body) {
      // 可以添加循环分析逻辑
      (void)item.func->body;  // 避免未使用警告
    }
  }
}
