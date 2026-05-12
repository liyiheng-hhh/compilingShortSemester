#include "common.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>

using namespace std;

int alignTo(int value, int align) {
  return (value + align - 1) / align * align;
}

uint32_t floatBits(float value) {
  uint32_t bits = 0;
  static_assert(sizeof(bits) == sizeof(value), "float must be 32-bit");
  memcpy(&bits, &value, sizeof(bits));
  return bits;
}

string escapeAsmString(const string &s) {
  string out;
  for (unsigned char c : s) {
    switch (c) {
    case '\n':
      out += "\\n";
      break;
    case '\t':
      out += "\\t";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '"':
      out += "\\\"";
      break;
    case '\0':
      out += "\\000";
      break;
    default:
      if (isprint(c)) {
        out.push_back(static_cast<char>(c));
      } else {
        char buf[8];
        snprintf(buf, sizeof(buf), "\\%03o", c);
        out += buf;
      }
      break;
    }
  }
  return out;
}

int product(const vector<int> &dims, size_t from) {
  int64_t result = 1;
  for (size_t i = from; i < dims.size(); ++i) {
    result *= dims[i];
  }
  if (result > numeric_limits<int>::max()) {
    throw CompileError("array is too large");
  }
  return static_cast<int>(result);
}

string readFile(const string &path) {
  ifstream in(path, ios::binary);
  if (!in) {
    throw CompileError("cannot open input file: " + path);
  }
  ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

void writeFile(const string &path, const string &content) {
  ofstream out(path, ios::binary);
  if (!out) {
    throw CompileError("cannot open output file: " + path);
  }
  out << content;
}
