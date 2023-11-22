add_executable(off_parser_example main.cpp)
target_compile_features(off_parser_example INTERFACE cxx_std_20)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(argparse)

target_link_libraries(off_parser_example
    PRIVATE
        off_parser
        fmt::fmt argparse::argparse
)
