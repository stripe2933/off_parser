/*
    Requires C++20 for concepts.

    Usage:

    #include <vector>
    #include <cmath>
    #include <iostream>

    int main() {
        auto calculate_sin = []() -> void {
            for (std::size_t i = 0; i < 10000; ++i){
                double x = static_cast<double>(i);
                double y = std::sin(x);
            }
        };
        auto elapsed_ms = benchmark(calculate_sin);
        std::cout << elapsed_ms << '\n'; // Possible output: 4.2e-05ms

        auto calculate_cos = []() -> std::vector<double> {
            std::vector<double> yy;
            yy.reserve(10000);

            for (std::size_t i = 0; i < 10000; ++i){
                double x = static_cast<double>(i);
                double y = std::sin(x);

                yy.push_back(y);
            }
            return yy;
        };

        auto [yy, elapsed_us] = benchmark_with_result<float, std::micro>(calculate_cos);
        std::cout << yy.size() << ' ' << elapsed_us << '\n'; // Possible output: 10000 44.208µs
    }
*/


#pragma once

#include <concepts>
#include <ratio>
#include <type_traits>
#include <chrono>
#include <functional>
#include <utility>

template <typename Unit = float, typename Ratio = std::milli, std::invocable Fn>
auto benchmark(Fn &&func) noexcept(std::is_nothrow_invocable_v<Fn>)
    -> std::chrono::duration<Unit, Ratio>
{
    const auto start = std::chrono::high_resolution_clock::now();
    std::invoke(std::forward<Fn>(func));
    const auto end = std::chrono::high_resolution_clock::now();
    return end - start;
}

template <typename Unit = float, typename Ratio = std::milli, std::invocable Fn>
auto benchmark_with_result(Fn &&func) noexcept(std::is_nothrow_invocable_v<Fn>)
    -> std::pair<std::invoke_result_t<Fn>, std::chrono::duration<Unit, Ratio>>
{
    const auto start = std::chrono::high_resolution_clock::now();
    auto result = std::invoke(std::forward<Fn>(func));
    const auto end = std::chrono::high_resolution_clock::now();
    return { std::move(result), end - start };
}