#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

struct CompileError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

int alignTo(int value, int align);
std::uint32_t floatBits(float value);
std::string escapeAsmString(const std::string &s);
std::string readFile(const std::string &path);
void writeFile(const std::string &path, const std::string &content);
int product(const std::vector<int> &dims, std::size_t from = 0);
