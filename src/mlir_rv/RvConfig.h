#pragma once

#include <cstdlib>
#include <cstring>
#include <string>

namespace sys::rv {

inline bool envEnabled(const char *name, bool fallback = true) {
  const char *v = std::getenv(name);
  if (!v || !v[0]) return fallback;
  if (std::strcmp(v, "0") == 0 || std::strcmp(v, "false") == 0) return false;
  return true;
}

inline bool envEnabledSuffix(const char *suffix, bool fallback = true) {
  return envEnabled((std::string("SYSY_RV_") + suffix).c_str(), fallback);
}

}  // namespace sys::rv
