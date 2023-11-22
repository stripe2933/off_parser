//
// Created by gomkyung2 on 11/20/23.
//

/*
 * Map runtime value to compile-time type, using `std::type_identity`.
 *
 * Usage:
 *
 * #include <variant>
 * #include <print>
 * #include <type_traits>
 *
 * struct visitor{
 *     constexpr void operator()(std::monostate) const noexcept{
 *         std::println("std::monostate");
 *     }
 *
 *     template <typename T>
 *     constexpr void operator()(std::type_identity<T>) const noexcept{
 *         if constexpr (std::is_same_v<T, int>) {
 *             std::println("int");
 *         }
 *         else if constexpr (std::is_same_v<T, float>) {
 *             std::println("float");
 *         }
 *         else if constexpr (std::is_same_v<T, double>) {
 *             std::println("double");
 *         }
 *     }
 * };
 *
 * #include <cassert>
 * #include <cstdlib>
 *
 * int main(int argc, char **argv){
 *     assert(argc == 2);
 *     int n = std::atoi(argv[1]);
 *
 *     // Map int value to type: [0 -> int, 1 -> float, 2 -> double]
 *     // You can explicitly specify the type mapping, like: `type_mapping<int, int>(0)`.
 *     constexpr type_mapper tm_int {
 *         make_type_mapping<int>(0),   // The type inside the parentheses must be same for all mappings.
 *         make_type_mapping<float>(1),
 *         make_type_mapping<double>(2)
 *     };
 *
 *     // When using method `map`, if the given value `n` is neither 0, 1, nor 2, `std::monostate` will be printed.
 *     // The result type is `variant<monostate, type_identity<int>, type_identity<float>, type_identity<double>>`.
 *     auto map_result = tm_int.map(n);
 *     std::visit(visitor{}, map_result);
 *
 *     // When using method `static_map`, if the given value `n` is neither 0, 1, nor 2, `std::runtime_error`
 *     // will be thrown. The result type is `variant<type_identity<int>, type_identity<float>, type_identity<double>>`.
 *     auto static_map_result = tm_int.static_map(n);
 *     std::visit(visitor{}, static_map_result);
 * }
 */

#pragma once

#include <tuple>
#include <variant>
#include <stdexcept>
#include <concepts>
#include <type_traits>

#define FWD(x) std::forward<decltype(x)>(x)

template <typename T, typename V>
struct type_mapping{
    using type = T;
    using mapper_type = V;

    V mapper;
};

template <typename T, typename V>
constexpr auto make_type_mapping(V &&value) noexcept -> type_mapping<T, V>{
    return { FWD(value) };
}

template <typename MappingV, typename... MappingTs>
class type_mapper{
private:
    /**
     * @brief Get a new tuple containing all but first element of a tuple, e.g. (a, b, c) -> (b, c).
     * @tparam Ts1
     * @tparam Ts
     * @param tuple The tuple to be processed.
     * @return A new tuple containing all but first element of a tuple.
     * @example
     * @code
     * skip_front(std::tuple{1, 2, 3}) // -> std::tuple{2, 3}
     * @endcode
     */
    template <typename Ts1, typename... Ts>
    static constexpr auto skip_front(std::tuple<Ts1, Ts...> tuple) noexcept -> std::tuple<Ts...>{
        return std::apply(
            [](Ts1 &&, Ts &&...tail) -> std::tuple<Ts...> {
                return { FWD(tail)... };
            },
            std::move(tuple)
        );
    }

    template <typename V, typename Mapping, typename ...Mappings>
    constexpr auto map(V &&value, std::tuple<Mapping, Mappings...> mappings) const noexcept
        -> std::variant<std::monostate,
                        std::type_identity<typename Mapping::type>,
                        std::type_identity<typename Mappings::type>...>
    {
        if (value == std::get<0>(mappings).mapper) {
            return std::type_identity<typename Mapping::type>{};
        }

        if constexpr (sizeof...(Mappings) == 0){
            return std::monostate{};
        }
        else{
            return std::visit(
                [](auto &&value) -> std::variant<std::monostate,
                                                 std::type_identity<typename Mapping::type>,
                                                 std::type_identity<typename Mappings::type>...>
                {
                    return FWD(value);
                },
                map(FWD(value), skip_front(mappings))
            );
        }
    }

    template <typename V, typename Mapping, typename ...Mappings>
    constexpr auto static_map(V &&value, std::tuple<Mapping, Mappings...> mappings) const
        -> std::variant<std::type_identity<typename Mapping::type>,
                        std::type_identity<typename Mappings::type>...>
    {
        if (value == std::get<0>(mappings).mapper) {
            return std::type_identity<typename Mapping::type>{};
        }

        if constexpr (sizeof...(Mappings) == 0){
            throw std::runtime_error { "No mapping found for the given value." };
        }
        else{
            return std::visit(
                [](auto &&value) -> std::variant<std::type_identity<typename Mapping::type>,
                                                 std::type_identity<typename Mappings::type>...>
                {
                    return FWD(value);
                },
                static_map(FWD(value), skip_front(mappings))
            );
        }
    }

public:
    std::tuple<type_mapping<MappingTs, MappingV>...> mappings;

    constexpr type_mapper(type_mapping<MappingTs, MappingV>... mappings) noexcept
        : mappings { std::move(mappings)... } { }

    template <typename V>
    constexpr auto map(V &&value) const noexcept -> std::variant<std::monostate, std::type_identity<MappingTs>...>{
        return map<V, type_mapping<MappingTs, MappingV>...>(FWD(value), mappings);
    }

    template <typename V>
    constexpr auto static_map(V &&value) const -> std::variant<std::type_identity<MappingTs>...>{
        return static_map<V, type_mapping<MappingTs, MappingV>...>(FWD(value), mappings);
    }
};