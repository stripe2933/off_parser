//
// Created by gomkyung2 on 11/19/23.
//

#pragma once

#include <type_traits>

namespace off_parser::concepts{
    namespace details{
        template <typename, template <typename, typename...> typename>
        struct is_instance : std::false_type {};

        template <typename...Ts, template <typename, typename...> typename U>
        struct is_instance<U<Ts...>, U> : std::true_type {};
    }

    template <typename T, template <typename, typename...> typename U>
    concept instance_of = details::is_instance<T, U>::value;

    template <typename T, template <typename, typename...> typename... U>
    concept instance_of_either = std::disjunction_v<details::is_instance<T, U>...>;
}