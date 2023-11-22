//
// Created by gomkyung2 on 11/20/23.
//

#pragma once

#include <optional>

#include "vec.hpp"

namespace off_parser::geometry{
    // Vertex type.
    template <concepts::vec_type PositionT>
    struct vertex{
        using position_type = PositionT;

        PositionT position;
    };

    template <concepts::vec_type PositionT, concepts::vec_type ColorT>
    struct colored_vertex : vertex<PositionT>{
        using color_type = ColorT;

        color_type color;
    };

    template <concepts::vec_type PositionT, concepts::vec_type ColorT>
    struct optional_colored_vertex : vertex<PositionT>{
        using color_type = ColorT;

        std::optional<color_type> color;
    };

    namespace concepts{
        template <typename T>
        concept vertex_type = off_parser::concepts::instance_of_either<
            std::remove_cvref_t<T>,
            vertex, colored_vertex, optional_colored_vertex
        >;
    }
}