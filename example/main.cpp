#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>

#include <argparse/argparse.hpp>
#include <off_parser/parser.hpp>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <fmt/std.h>
#include <fmt/ostream.h>
#include <fmt/color.h>

#include "benchmark.hpp"
#include "omitted.hpp"
#include "formatter.hpp"
#include "type_mapper.hpp"

using namespace off_parser;

template <typename... Args>
void println_warning(fmt::format_string<Args...> fmt, Args &&...args){
    fmt::print(fg(fmt::terminal_color::yellow), "[WARNING] ");
    fmt::print(fmt, std::forward<Args>(args)...);
    fmt::print("\n");
}

[[noreturn]] void unreachable(){
    fmt::println(std::cerr, "Logic error: unreachable code is reached.");
    std::exit(-1);
}

template <typename T>
using tmi2 /* type mapping with 2 ints */ = type_mapping<T, std::array<int, 2>>;

void parse_file_and_print_result(const std::filesystem::path &path, const argparse::ArgumentParser &program){
    using namespace off_parser::geometry;

    int parse_vertex_color = [&]() -> int{
        if (program.get<bool>("-nvc")) return 0;
        if (program.get<bool>("-vc")) return 1;
        if (program.get<bool>("-ovc")) return 2;
        unreachable();
    }();
    int n_vertex_color_channel = (parse_vertex_color == 0) ? -1 : program.get<int>("-vcc");

    constexpr type_mapper vcmap {
        tmi2<vertex                 <vec3<double>              >>({ 0, -1 }),
        tmi2<colored_vertex         <vec3<double>, vec3<double>>>({ 1,  3 }),
        tmi2<colored_vertex         <vec3<double>, vec4<double>>>({ 1,  4 }),
        tmi2<optional_colored_vertex<vec3<double>, vec3<double>>>({ 2,  3 }),
        tmi2<optional_colored_vertex<vec3<double>, vec4<double>>>({ 2,  4 }),
    };

    int parse_face_color = [&]() -> int{
        if (program.get<bool>("-nfc")) return 0;
        if (program.get<bool>("-fc")) return 1;
        if (program.get<bool>("-ofc")) return 2;
        unreachable();
    }();
    int n_face_color_channel = (parse_face_color == 0) ? -1 : program.get<int>("-fcc");

    constexpr type_mapper fcmap {
        tmi2<face                 <std::uint16_t              >>({ 0, -1 }),
        tmi2<colored_face         <std::uint16_t, vec3<double>>>({ 1,  3 }),
        tmi2<colored_face         <std::uint16_t, vec4<double>>>({ 1,  4 }),
        tmi2<optional_colored_face<std::uint16_t, vec3<double>>>({ 2,  3 }),
        tmi2<optional_colored_face<std::uint16_t, vec4<double>>>({ 2,  4 }),
    };

    std::visit(
        [&]<typename VertexProxy, typename FaceProxy>(VertexProxy, FaceProxy){
            auto [result, elapsed] = benchmark_with_result([&]() {
                std::ifstream input { path };
                return parse<mesh<typename VertexProxy::type, typename FaceProxy::type>>(std::move(input));
            });

            fmt::print(fg(fmt::terminal_color::green), "[RESULT]\n");
            fmt::println(
                "vertices = {::s},\n"
                "faces = {::s},\n"
                "n_edges = {}",
                omitted {
                    result.vertices
                    | std::views::transform([]<typename VertexT>(const VertexT &vertex) -> std::string {
                        if constexpr (off_parser::concepts::instance_of<
                                std::remove_cvref_t<VertexT>,
                                geometry::vertex>)
                        {
                            return fmt::format("{:.2f}", vertex.position);
                        }
                        else{
                            return fmt::format("Vertex(p={:.2f}, c={:.2f})", vertex.position, vertex.color);
                        }
                    })
                },
                omitted {
                    result.faces
                    | std::views::transform([]<typename FaceT>(const FaceT &face) -> std::string {
                        if constexpr (off_parser::concepts::instance_of<
                                std::remove_cvref_t<FaceT>,
                                geometry::face>)
                        {
                            return fmt::format("{}", face.vertex_indices);
                        }
                        else{
                            return fmt::format("Face(vi={}, c={})", face.vertex_indices, face.color);
                        }
                    })
                },
                result.n_edges
            );
            fmt::println("Elapsed: {}", elapsed);
        },
        vcmap.static_map(std::array { parse_vertex_color, n_vertex_color_channel }),
        fcmap.static_map(std::array { parse_face_color, n_face_color_channel })
    );
}

/**
 * @brief Check if the value is one of the valid values.
 * @tparam T
 * @param value
 * @param valid_values
 * @return \p true if \p value is one of the \p valid_values, \p false otherwise.
 */
template <std::equality_comparable T>
constexpr bool is_one_of(T value, std::initializer_list<T> valid_values){
    return std::ranges::find(valid_values, value) != valid_values.end();
}

int main(int argc, char **argv) {
    argparse::ArgumentParser program { "OFF parser" };
    program.add_argument("file")
        .help("OFF file to parse");

    auto &vertex_color_group = program.add_mutually_exclusive_group(true);
    vertex_color_group.add_argument("-nvc", "--no-vertex-color")
        .default_value(false)
        .implicit_value(true)
        .help("Disable vertex color parsing.");
    vertex_color_group.add_argument("-vc", "--vertex-color")
        .default_value(false)
        .implicit_value(true)
        .help("Enable vertex color parsing.");
    vertex_color_group.add_argument("-ovc", "--optional-vertex-color")
        .default_value(false)
        .implicit_value(true)
        .help("Parse vertex color if presented.");

    program.add_argument("-vcc", "--vertex-color-channels")
        .default_value(3)
        .scan<'i', int>()
        .help("Number of vertex color channels. [3 -> RGB, 4 -> RGBA]");

    auto &face_color_group = program.add_mutually_exclusive_group(true);
    face_color_group.add_argument("-nfc", "--no-face-color")
        .default_value(false)
        .implicit_value(true)
        .help("Disable face color parsing.");
    face_color_group.add_argument("-fc", "--face-color")
        .default_value(false)
        .implicit_value(true)
        .help("Enable face color parsing.");
    face_color_group.add_argument("-ofc", "--optional-face-color")
        .default_value(false)
        .implicit_value(true)
        .help("Parse face color if presented.");

    program.add_argument("-fcc", "--face-color-channels")
        .default_value(3)
        .scan<'i', int>()
        .help("Number of face color channels. [3 -> RGB, 4 -> RGBA]");

    // Argument validation.
    try{
        program.parse_args(argc, argv);

        if (!is_one_of(program.get<int>("-vcc"), { 3, 4 })){
            throw std::invalid_argument { "The number of vertex color channel must be 3 (RGB) or 4 (RGBA)." };
        }
        if (!is_one_of(program.get<int>("-fcc"), { 3, 4 })){
            throw std::invalid_argument { "The number of face color channel must be 3 (RGB) or 4 (RGBA)." };
        }

        // Normally, color is not specified for BOTH the vertices and faces.
        if (program.get<bool>("-vc") && program.get<bool>("-fc")){
            throw std::invalid_argument { "OFF file should not have both colored vertex and colored face." };
        }
    }
    catch (const std::runtime_error &err){
        fmt::println(std::cerr, "{}\n{}", err.what(), program);
        return 1;
    }
    catch (const std::invalid_argument &err){
        fmt::println(std::cerr, "{}", err.what());
        return 2;
    }

    // Argument validation: not error, but warning.
    if (program.get<bool>("-nvc") && program.is_used("-vcc")){
        // No vertex color, but color channel is given by user.
        println_warning("The number of vertex color channel is given, but vertex color is disabled.");
    }
    if (program.get<bool>("-nfc") && program.is_used("-fcc")){
        // No face color, but color channel is given by user.
        println_warning("The number of face color channel is given, but face color is disabled.");
    }

    parse_file_and_print_result(program.get<std::string>("file"), program);
}