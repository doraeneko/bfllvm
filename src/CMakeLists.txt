cmake_minimum_required(VERSION 3.16)
project(bfllvm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(LLVM REQUIRED CONFIG)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(bfllvm lexer.cpp parser.cpp code_gen.cpp driver.cpp)

llvm_map_components_to_libnames(LLVM_LIBS core orcjit native)

target_link_libraries(bfllvm ${LLVM_LIBS})


add_custom_target(
    pytest ALL
    COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_SOURCE_DIR}/tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Python pytest..."
    VERBATIM
)

# Ensure that pytest runs after building the project
add_dependencies(pytest bfllvm)