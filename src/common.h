#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

struct CompileError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

// truthy getenv：未设置、`""`、`"0"` 为 false（用于评测/本地二分，不影响默认行为）。
bool envFlagTruthy(const char *envName);

int alignTo(int value, int align);
std::uint32_t floatBits(float value);
std::string escapeAsmString(const std::string &s);
std::string readFile(const std::string &path);
void writeFile(const std::string &path, const std::string &content);
int product(const std::vector<int> &dims, std::size_t from = 0);
