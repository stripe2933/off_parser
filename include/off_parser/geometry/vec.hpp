//
// Created by gomkyung2 on 11/20/23.
//

#pragma once

#include <cstddef>

#include "../concepts.hpp"

namespace off_parser::geometry{
    // Vector type.
    template <typename T>
    struct vec3{
        using value_type = T;
        static constexpr std::size_t size = 3;

        T x, y, z;
    };

    template <typename T>
    struct vec4{
        using value_type = T;
        static constexpr std::size_t size = 4;

        T x, y, z, w;
    };

    namespace concepts{
        template <typename T>
        concept vec_type = off_parser::concepts::instance_of_either<
            std::remove_cvref_t<T>,
            vec3, vec4
        >;
    }

    template <std::size_t I, concepts::vec_type VecT>
        requires (I < VecT::size)
    [[nodiscard]] constexpr typename VecT::value_type &nth(VecT &v) noexcept {
        if constexpr (I == 0) {
            return v.x;
        }
        else if constexpr (I == 1) {
            return v.y;
        }
        else if constexpr (I == 2) {
            return v.z;
        }
        else if constexpr (I == 3) {
            return v.w;
        }
    }

    template <std::size_t I, concepts::vec_type VecT>
        requires (I < VecT::size)
    [[nodiscard]] constexpr const typename VecT::value_type &nth(const VecT &v) noexcept {
        if constexpr (I == 0) {
            return v.x;
        }
        else if constexpr (I == 1) {
            return v.y;
        }
        else if constexpr (I == 2) {
            return v.z;
        }
        else if constexpr (I == 3) {
            return v.w;
        }
    }
}