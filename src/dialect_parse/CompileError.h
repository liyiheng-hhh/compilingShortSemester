#ifndef DIALECT_COMPILE_ERROR_H
#define DIALECT_COMPILE_ERROR_H

#include <stdexcept>
#include <string>

namespace sys {

class CompileError : public std::runtime_error {
public:
  explicit CompileError(const std::string &msg): std::runtime_error(msg) {}
};

using ParseFailure = CompileError;

}

#endif  // DIALECT_COMPILE_ERROR_H
