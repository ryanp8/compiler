cmake_minimum_required(VERSION 3.20)

project(compiler LANGUAGES CXX)
 
# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/PEGTL)
 
add_executable(${PROJECT_NAME} src/main.cpp src/parser/parser.cpp src/parser/ast.cpp)
 
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE taocpp::pegtl)