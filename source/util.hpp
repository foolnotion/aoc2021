#ifndef UTIL_HPP
#define UTIL_HPP

#include <algorithm>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <fmt/color.h>
#include <fmt/format.h>

#include <Eigen/Eigen>
#include <gsl/gsl>

#define EXPECT(cond)                                                       \
    if (GSL_UNLIKELY(!(cond))) {                                           \
        fmt::print("Precondition {} failed at {}: {}\n",                   \
            fmt::format(fmt::fg(fmt::terminal_color::green), "{}", #cond), \
            __FILE__,                                                      \
            __LINE__);                                                     \
        gsl::details::terminate();                                         \
    }

#define ENSURE(cond)                                                       \
    if (GSL_UNLIKELY(!(cond))) {                                           \
        fmt::print("Precondition {} failed at {}: {}\n",                   \
            fmt::format(fmt::fg(fmt::terminal_color::green), "{}", #cond), \
            __FILE__,                                                      \
            __LINE__);                                                     \
        gsl::details::terminate();                                         \
    }

template <typename T>
static auto parse_number(std::string_view s) -> std::optional<T>
{
    static_assert(std::is_arithmetic<T>::value,
        "The return type must be arithmetic");
    T result;
    if (auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
        ec == std::errc()) {
        return { result };
    }
    return {};
}

template <typename T>
static auto parse_number(std::string const& s) -> std::optional<T>
{
    return parse_number<T>(std::string_view(s));
}

template <typename T>
static auto parse_number(char const* p) -> std::optional<T>
{
    return parse_number<T>(std::string_view(p));
}

static auto split(const std::string& s, char delimiter)
    -> std::vector<std::string>
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

template <typename T>
static auto to_vec(std::string const& s, char delimiter) -> std::vector<T>
{
    auto tokens = split(s, delimiter);
    std::vector<T> vec;
    for (const auto& t : tokens) {
        auto res = parse_number<T>(t);
        if (!res.has_value()) {
            throw std::runtime_error(fmt::format(
                "Error: cannot parse '{}' as an {}\n", t, typeid(T).name()));
        }
        vec.push_back(res.value());
    }
    return vec;
}

template <typename T, typename U = typename T::value_type>
auto as_map(T const& t) -> Eigen::Map<const Eigen::Matrix<U, Eigen::Dynamic, 1>>
{
    return Eigen::Map<const Eigen::Matrix<U, Eigen::Dynamic, 1, Eigen::ColMajor>>(
        t.data(), t.size());
}

template <typename T>
struct point {
    T x;
    T y;
    T z;

    template <size_t I>
    auto get() & -> auto&
    {
        if constexpr (I == 0) {
            return x;
        } else if constexpr (I == 1) {
            return y;
        } else if constexpr (I == 2) {
            return z;
        }
    }

    template <size_t I>
    auto get() const& -> auto const&
    {
        if constexpr (I == 0) {
            return x;
        } else if constexpr (I == 1) {
            return y;
        } else if constexpr (I == 2) {
            return z;
        }
    }

    template <size_t I>
    auto get() && -> auto&&
    {
        if constexpr (I == 0) {
            return x;
        } else if constexpr (I == 1) {
            return y;
        } else if constexpr (I == 2) {
            return z;
        }
    }

    auto operator==(point<T> other) const noexcept -> bool
    {
        return x == other.x && y == other.y && z == other.z;
    }

    auto operator<(point<T> other) const noexcept -> bool
    {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }
};

namespace std {
template <typename T>
struct tuple_size<point<T>> : std::integral_constant<size_t, 3> {
};
template <typename T>
struct tuple_element<0, point<T>> {
    using type = T;
};
template <typename T>
struct tuple_element<1, point<T>> {
    using type = T;
};
template <typename T>
struct tuple_element<2, point<T>> {
    using type = T;
};
} // namespace std

#endif
