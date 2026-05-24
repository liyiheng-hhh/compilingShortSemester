#include "dialect_fallback.h"

#include "common.h"

#include <string>

namespace {

std::string basenameOnly(const std::string &path) {
  const auto p = path.find_last_of("/\\");
  return p == std::string::npos ? path : path.substr(p + 1);
}

}  // namespace

bool sys::dialectPreferLegacyPipeline(const std::string &source,
                                      const std::string &inputPath) {
  (void)source;
  (void)basenameOnly(inputPath);
  if (envFlagTruthy("SYSY_CC_FORCE_DIALECT_PIPELINE"))
    return false;
  if (envFlagTruthy("SYSY_CC_NO_DIALECT_LEGACY_FALLBACK"))
    return false;
  return false;
}
