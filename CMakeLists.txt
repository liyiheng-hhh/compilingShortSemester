cmake_minimum_required(VERSION 3.16)

project(ToyCCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(toyc
    src/main.cpp
)

target_compile_options(toyc PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

enable_testing()

add_test(
    NAME smoke_tests
    COMMAND python ${PROJECT_SOURCE_DIR}/run_tests.py tests/smoke/ --no-build
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME all_tests
    COMMAND python ${PROJECT_SOURCE_DIR}/run_tests.py --no-build
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME opt_tests
    COMMAND python ${PROJECT_SOURCE_DIR}/run_tests.py --opt --no-build
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

