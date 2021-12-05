#ifndef ADVENT_HPP
#define ADVENT_HPP

#include <fmt/color.h>
#include <fmt/format.h>
#include <gsl/gsl_assert>
#include <string>
#include <vector>

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

namespace util {
inline void tokenize(std::string const& str, char delim, std::vector<std::string>& out)
{
    size_t start { 0 };
    size_t end { 0 };

    constexpr size_t npos = std::string::npos;

    while ((start = str.find_first_not_of(delim, end)) != npos) {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}
} // namespace util

auto day01(int argc, char** argv) -> int;
auto day02(int argc, char** argv) -> int;
auto day03(int argc, char** argv) -> int;
auto day04(int argc, char** argv) -> int;
auto day05(int argc, char** argv) -> int;

#endif
