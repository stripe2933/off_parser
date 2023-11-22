add_executable(off_parser_example main.cpp)
target_compile_features(off_parser_example INTERFACE cxx_std_20)

add_subdirectory(extlibs/fmt)
add_subdirectory(extlibs/argparse)
target_link_libraries(off_parser_example PRIVATE off_parser fmt::fmt argparse::argparse)
