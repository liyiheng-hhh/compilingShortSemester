cmake_minimum_required(VERSION 3.16)

project(ToyCCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(toyc
    src/main.cpp
)

target_compile_options(toyc PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O2 -Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/O2 /W4>
)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME regressions
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_regressions.py $<TARGET_FILE:toyc>
        )
        add_test(NAME ctfe_differential
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_ctfe_differential.py $<TARGET_FILE:toyc>
        )
    endif()
endif()
