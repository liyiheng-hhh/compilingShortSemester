#pragma once

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace toyc {

struct Options {
    bool optimize = false;
    bool enableCtfe = true;
};

inline Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-opt") {
            options.optimize = true;
        } else if (arg == "-fno-ctfe") {
            options.enableCtfe = false;
        }
    }
    return options;
}

inline std::string readStdin() {
    return {std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
}

struct SourceLocation {
    int line = 1;
    int column = 1;
    std::size_t offset = 0;
};

[[noreturn]] inline void fail(const SourceLocation& loc, const std::string& message) {
    std::ostringstream out;
    out << loc.line << ':' << loc.column << ": " << message;
    throw std::runtime_error(out.str());
}

inline std::int32_t toInt32(std::int64_t value) {
    return static_cast<std::int32_t>(value);
}

inline std::int64_t printableI32(std::int32_t value) {
    return static_cast<std::int64_t>(value);
}

inline bool fitsI12(int value) {
    return value >= -2048 && value <= 2047;
}

inline std::optional<int> powerOfTwoShift(std::int64_t value) {
    if (value == 0) {
        return std::nullopt;
    }
    std::uint64_t magnitude = value < 0 ? static_cast<std::uint64_t>(-value)
                                        : static_cast<std::uint64_t>(value);
    if ((magnitude & (magnitude - 1)) != 0 || magnitude > (1ull << 31)) {
        return std::nullopt;
    }
    int shift = 0;
    while (magnitude > 1) {
        magnitude >>= 1;
        ++shift;
    }
    return shift;
}

inline int alignTo16(int value) {
    return (value + 15) / 16 * 16;
}

inline std::string functionLabel(const std::string& name) {
    if (name == "main") {
        return "main";
    }
    return "fn_" + name;
}

inline std::string globalLabel(const std::string& name) {
    return "g_" + name;
}

} // namespace toyc
