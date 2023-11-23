cmake_minimum_required(VERSION 3.26)
project(
    off_parser
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "A C++ library for parsing OFF files."
    HOMEPAGE_URL "https://github.com/stripe2933/off-parser"
)

add_library(off_parser INTERFACE)
target_compile_features(off_parser INTERFACE cxx_std_20)
target_include_directories(off_parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(example)
endif()
