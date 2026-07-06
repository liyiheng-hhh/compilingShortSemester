#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

struct Options {
    bool optimize = false;
};

Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-opt") {
            options.optimize = true;
        }
    }
    return options;
}

std::string readStdin() {
    return {std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
}

void emitInitialAssembly(std::ostream& out) {
    out << "    .text\n";
    out << "    .globl main\n";
    out << "main:\n";
    out << "    li a0, 0\n";
    out << "    ret\n";
}

} // namespace

int main(int argc, char** argv) {
    const Options options = parseOptions(argc, argv);
    const std::string source = readStdin();

    (void)options;
    (void)source;

    emitInitialAssembly(std::cout);
    return 0;
}
