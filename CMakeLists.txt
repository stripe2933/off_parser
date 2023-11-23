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

# Set OFF_PARSER_STANDALONE_BUILD flag if it is unset and project is top-level project.
if (NOT DEFINED OFF_PARSER_STANDALONE_BUILD)
    set(OFF_PARSER_STANDALONE_BUILD PROJECT_IS_TOP_LEVEL)
endif()

if (OFF_PARSER_STANDALONE_BUILD)
    add_subdirectory(example)
endif()
