#ifndef UTIL_HPP
#define UTIL_HPP

#include <fmt/color.h>
#include <fmt/format.h>

#include <Eigen/Eigen>

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
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <type_traits>

#include <gsl/gsl>

#define EXPECT(cond) \
    if(GSL_UNLIKELY(!(cond))) \
    { \
        fmt::print("Precondition {} failed at {}: {}\n", fmt::format(fmt::fg(fmt::terminal_color::green), "{}", #cond), __FILE__, __LINE__); \
        gsl::details::terminate(); \
    } 

#define ENSURE(cond) \
    if(GSL_UNLIKELY(!(cond))) \
    { \
        fmt::print("Precondition {} failed at {}: {}\n", fmt::format(fmt::fg(fmt::terminal_color::green), "{}", #cond), __FILE__, __LINE__); \
        gsl::details::terminate(); \
    }

template <typename T>
static std::optional<T> parse_number(std::string_view s)
{
    static_assert(std::is_arithmetic_v<T>);
    T result;
    if (auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), result); ec == std::errc()) {
        return { result };
    }
    return {};
}

template <typename T>
static std::optional<T> parse_number(std::string const& s)
{
    return parse_number<T>(std::string_view(s));
}

template <typename T>
static std::optional<T> parse_number(char const* p)
{
    return parse_number<T>(std::string_view(p));
}

static std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

template <typename T>
static std::vector<T> to_vec(const std::string& s, char delimiter)
{
    auto tokens = split(s, delimiter);
    std::vector<T> vec;
    for (const auto& t : tokens) {
        if (auto res = parse_number<T>(t); !res.has_value()) {
            throw new std::runtime_error(fmt::format("Error: cannot parse '{}' as an {}\n", t, typeid(T).name()));
        } else {
            vec.push_back(res.value());
        }
    }
    return vec;
}

template<typename T, typename U = typename T::value_type>
Eigen::Map<const Eigen::Matrix<U, Eigen::Dynamic, 1>> as_map(T const& t)
{
    return Eigen::Map<const Eigen::Matrix<U, Eigen::Dynamic, 1, Eigen::ColMajor>>(t.data(), t.size());
}

template<typename T>
struct point {
    T x;
    T y;
    T z;

    template <size_t I>
    auto& get() & {
        if constexpr (I == 0) return x;
        else if constexpr (I == 1) return y;
        else if constexpr (I == 2) return z;
    }

    template <size_t I>
    auto const& get() const& {
        if constexpr (I == 0) return x;
        else if constexpr (I == 1) return y;
        else if constexpr (I == 2) return z;
    }

    template <size_t I>
    auto&& get() && {
        if constexpr (I == 0) return x;
        else if constexpr (I == 1) return y;
        else if constexpr (I == 2) return z;
    }

    bool operator==(point<T> other) const noexcept
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(point<T> other) const noexcept
    {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }
};

namespace std {
    template <typename T> struct tuple_size<point<T>> : std::integral_constant<size_t, 3> { };
    template <typename T> struct tuple_element<0,point<T>> { using type = T; };
    template <typename T> struct tuple_element<1,point<T>> { using type = T; };
    template <typename T> struct tuple_element<2,point<T>> { using type = T; };
}

#endif
