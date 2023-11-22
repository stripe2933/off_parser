//
// Created by gomkyung2 on 11/20/23.
//

#pragma once

#include "off_parser/geometry/vec.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <argparse/argparse.hpp>

using namespace off_parser;

// Make vec3<T> be able to format.
template <typename T, typename CharT>
struct fmt::formatter<geometry::vec3<T>, CharT> : formatter<T, CharT>{
    constexpr auto format(const geometry::vec3<T> &vec, format_context &ctx) const{
        fmt::format_to(ctx.out(), "(");
        formatter<T, CharT>::format(vec.x, ctx);
        fmt::format_to(ctx.out(), ",");
        formatter<T, CharT>::format(vec.y, ctx);
        fmt::format_to(ctx.out(), ",");
        formatter<T, CharT>::format(vec.z, ctx);
        return fmt::format_to(ctx.out(), ")");
    }
};

// Make vec4<T> be able to format.
template <typename T, typename CharT>
struct fmt::formatter<geometry::vec4<T>, CharT> : formatter<T, CharT>{
    constexpr auto format(const geometry::vec4<T> &vec, format_context &ctx) const{
        fmt::format_to(ctx.out(), "(");
        formatter<T, CharT>::format(vec.x, ctx);
        fmt::format_to(ctx.out(), ",");
        formatter<T, CharT>::format(vec.y, ctx);
        fmt::format_to(ctx.out(), ",");
        formatter<T, CharT>::format(vec.z, ctx);
        fmt::format_to(ctx.out(), ",");
        formatter<T, CharT>::format(vec.w, ctx);
        return fmt::format_to(ctx.out(), ")");
    }
};

// Make ArgumentParser be able to format.
template <>
struct fmt::formatter<argparse::ArgumentParser> : ostream_formatter { };