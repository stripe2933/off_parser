//
// Created by gomkyung2 on 11/19/23.
//

#pragma once

#include <istream>
#include <array>

#include "geometry/mesh.hpp"
#include "concepts.hpp"

#define OFF_PARSER_INDEX_SEQUENCE(Is, N, Expression) [&]<std::size_t... Is>(std::index_sequence<Is...>){ Expression }(std::make_index_sequence<N>{})

namespace off_parser{
    static void ignore_until_newline(std::istream &input){
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    static void ignore_comment_or_empty_lines(std::istream &input){
        while (!input.eof()){
            switch (input.peek()){
                case '\n':
                    input.ignore();
                    break;
                case '#':
                    ignore_until_newline(input);
                    break;
                default:
                    return;
            }
        }
    }

    template <geometry::concepts::vec_type VecT>
    [[nodiscard]] VecT parse_vec(std::istream &input){
        VecT result;
        OFF_PARSER_INDEX_SEQUENCE(Is, VecT::size,
            (input >> ... >> geometry::nth<Is>(result));
        );
        return result;
    }

    template <geometry::concepts::vec_type VecT>
    [[nodiscard]] std::optional<VecT> parse_vec_within_line(std::istream &input){
        std::array<typename VecT::value_type, VecT::size> buffer;
        for (auto &elem : buffer){
            if (input.peek() == '\n'){
                return std::nullopt;
            }

            input >> elem;
        }

        return std::make_from_tuple<VecT>(buffer);
    }

    template <geometry::concepts::mesh_type MeshT>
    MeshT parse(std::istream &&input){
        // Ignore the first line (header).
        ignore_until_newline(input);

        ignore_comment_or_empty_lines(input);

        // Read number of vertices, faces, and edges.
        std::size_t n_vertices, n_faces;
        MeshT mesh;
        input >> n_vertices >> n_faces >> mesh.n_edges;

        // Reserve memory for vertices and faces.
        mesh.vertices.reserve(n_vertices);
        mesh.faces.reserve(n_faces);

        ignore_until_newline(input);

        // Parse vertices.
        for (std::size_t i = 0; i < n_vertices; ++i){
            ignore_comment_or_empty_lines(input);

            typename MeshT::vertex_type vertex;

            // Parse position.
            vertex.position = parse_vec<typename MeshT::vertex_type::position_type>(input);

            // Parse color if vertex type contains color field.
            if constexpr (concepts::instance_of<typename MeshT::vertex_type, geometry::colored_vertex>){
                vertex.color = parse_vec<typename MeshT::vertex_type::color_type>(input);
            }
            else if constexpr (concepts::instance_of<typename MeshT::vertex_type, geometry::optional_colored_vertex>){
                vertex.color = parse_vec_within_line<typename MeshT::vertex_type::color_type>(input);
            }

            mesh.vertices.push_back(vertex);

            ignore_until_newline(input);
        }

        // Parse faces.
        for (std::size_t i = 0; i < n_faces; ++i){
            ignore_comment_or_empty_lines(input);

            std::size_t n_vertices_in_face;
            input >> n_vertices_in_face;

            typename MeshT::face_type face;
            face.vertex_indices.reserve(n_vertices_in_face);

            // Parse positions.
            for (std::size_t j = 0; j < n_vertices_in_face && input.peek() != '\n'; ++j){
                typename MeshT::face_type::index_type index;
                input >> index;
                face.vertex_indices.push_back(index);
            }

            // Parse color if vertex type contains color field.
            if constexpr (concepts::instance_of<typename MeshT::face_type, geometry::colored_face>){
                face.color = parse_vec<typename MeshT::face_type::color_type>(input);
            }
            else if constexpr (concepts::instance_of<typename MeshT::face_type, geometry::optional_colored_face>){
                face.color = parse_vec_within_line<typename MeshT::face_type::color_type>(input);
            }

            mesh.faces.emplace_back(std::move(face));

            ignore_until_newline(input);
        }

        return mesh;
    }
}