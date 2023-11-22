//
// Created by gomkyung2 on 11/19/23.
//

/*
 * The struct `omit` and its formatter are used to print a range that have many elements. It omits the elements in the
 * middle of the range and prints only the first `n_head` (default: 2) and the last `n_tail` (default: 2) elements. You
 * may set `show_size` to `true` (default: `true`) to print the size of the range (if it is sized).
 *
 * Example:
 * std::vector nums { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
 * fmt::println("{}", omitted { nums | std::views::all }); // -> "[1, 2, ..., 9, 10] (size=10)"
 *
 * In the above example, we use `std::views::all` to make `nums` as view, because assigning `nums` to `omitted` will
 * copy the whole vector, which is not efficient.
 *
 * The constraint for view is it must be input range and either sized range or bidirectional range.
 * If it is sized range, the last `n_tail` elements can be obtained by
 *     `nums | drop(nums.size() - n_tail)`.
 * If it is bidirectional range, it would be
 *     `nums | reverse | take(n_tail) | reverse`.
 * If it is nor sized range nor bidirectional range, there's no way to get the last `n_tail` elements, therefore it
 * would be an error.
 *
 * Example:
 * std::list nums { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
 * fmt::println("{}", omitted { nums | std::views::filter([](int n) { return n % 2 == 0; }) });
 * // -> "[2, 4, ..., 8, 10]"
 *
 * In the above example, the view is bidirectional range and not sized range, so it can be printed properly, but the
 * size is not shown.
 */

#pragma once

#include <ranges>

#include <fmt/ranges.h>

template <std::ranges::view ViewT>
    requires
        std::ranges::input_range<ViewT> &&
        (std::ranges::sized_range<ViewT> || std::ranges::bidirectional_range<ViewT>)
struct omitted{
    ViewT view;
    std::size_t n_head = 2;
    std::size_t n_tail = 2;
    bool show_size = true;

    constexpr omitted(ViewT view) noexcept(std::is_nothrow_copy_assignable_v<ViewT>) : view { view } {

    }
};

template <typename ViewT, typename CharT>
struct fmt::formatter<omitted<ViewT>, CharT> : range_formatter<std::ranges::range_value_t<ViewT>, CharT>{
    constexpr formatter() noexcept{
        range_formatter<std::ranges::range_value_t<ViewT>, CharT>::set_brackets("", "");
    }

    template <typename FormatContext>
    constexpr auto format(omitted<ViewT> x, FormatContext &ctx) const{
        if constexpr (std::ranges::sized_range<ViewT>){
            fmt::format_to(ctx.out(), "[");

            if (x.view.size() <= x.n_head + x.n_tail){
                // If the size of the range is less than the number of elements to omit, just print the whole range.
                range_formatter<std::ranges::range_value_t<ViewT>, CharT>::format(x.view, ctx);
            }
            else{
                auto head = x.view | std::views::take(x.n_head);
                range_formatter<std::ranges::range_value_t<ViewT>, CharT>::format(head, ctx);

                fmt::format_to(ctx.out(), ", ..., ");

                auto tail = x.view | std::views::drop(x.view.size() - x.n_tail);
                range_formatter<std::ranges::range_value_t<ViewT>, CharT>::format(tail, ctx);
            }

            if (x.show_size){
                return fmt::format_to(ctx.out(), "] (size={})", x.view.size());
            }
            else{
                return fmt::format_to(ctx.out(), "]");
            }
        }
        else{ // By the constraint of omitted, the range must be bidirectional.
            fmt::format_to(ctx.out(), "[");

            auto head = x.view | std::views::take(x.n_head);
            range_formatter<std::ranges::range_value_t<ViewT>, CharT>::format(head, ctx);

            fmt::format_to(ctx.out(), ", ..., ");

            auto tail = x.view | std::views::reverse | std::views::take(x.n_tail) | std::views::reverse;
            range_formatter<std::ranges::range_value_t<ViewT>, CharT>::format(tail, ctx);

            return fmt::format_to(ctx.out(), "]");
        }
    }
};
