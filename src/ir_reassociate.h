#pragma once

#include "ir.h"

// 加法链重关联（Sisyphus Reassociate 算法，支配树遍历）
bool irReassociate(IRFunction &fn);
