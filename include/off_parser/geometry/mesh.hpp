//
// Created by gomkyung2 on 11/20/23.
//

#pragma once

#include <vector>

#include "vertex.hpp"
#include "face.hpp"

#include "../concepts.hpp"

namespace off_parser::geometry {
    // Mesh type.
    template <concepts::vertex_type VertexT, concepts::face_type FaceT>
    struct mesh{
        using vertex_type = VertexT;
        using face_type = FaceT;

        std::vector<vertex_type> vertices;
        std::vector<face_type> faces;
        std::size_t n_edges;
    };

    namespace concepts{
        template <typename T>
        concept mesh_type = off_parser::concepts::instance_of<
            std::remove_cvref_t<T>,
            mesh
        >;
    }
}