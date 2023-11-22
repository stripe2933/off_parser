//
// Created by gomkyung2 on 11/20/23.
//

#pragma once

#include <concepts>
#include <vector>

#include "vertex.hpp"

namespace off_parser::geometry {
    // Face type.
    template <std::integral IndexT>
    struct face {
        using index_type = IndexT;

        std::vector<index_type> vertex_indices;
    };

    template <std::integral T, concepts::vec_type ColorT>
    struct colored_face : face<T> {
        using color_type = ColorT;

        color_type color;
    };

    template<std::integral T, concepts::vec_type ColorT>
    struct optional_colored_face : face<T> {
        using color_type = ColorT;

        std::optional<color_type> color;
    };

    namespace concepts{
        template <typename T>
        concept face_type = off_parser::concepts::instance_of_either<
            std::remove_cvref_t<T>,
            face, colored_face, optional_colored_face
        >;
    }
}