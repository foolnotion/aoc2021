#ifndef ADVENT_HPP
#define ADVENT_HPP

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <scn/scn.h>
#include <string>
#include <vector>

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

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

template<typename T>
inline auto sgn(T value) -> int
{
    return (T{0} < value) - (value < T{0});
}
} // namespace util

auto day01(int argc, char** argv) -> int;
auto day02(int argc, char** argv) -> int;
auto day03(int argc, char** argv) -> int;
auto day04(int argc, char** argv) -> int;
auto day05(int argc, char** argv) -> int;
auto day06(int argc, char** argv) -> int;
auto day07(int argc, char** argv) -> int;
auto day08(int argc, char** argv) -> int;
auto day09(int argc, char** argv) -> int;
auto day10(int argc, char** argv) -> int;
auto day11(int argc, char** argv) -> int;
auto day12(int argc, char** argv) -> int;
auto day13(int argc, char** argv) -> int;
auto day14(int argc, char** argv) -> int;
auto day15(int argc, char** argv) -> int;
auto day16(int argc, char** argv) -> int;
auto day17(int argc, char** argv) -> int;
auto day18(int argc, char** argv) -> int;
auto day19(int argc, char** argv) -> int;
auto day20(int argc, char** argv) -> int;
auto day21(int argc, char** argv) -> int;
auto day22(int argc, char** argv) -> int;
auto day23(int argc, char** argv) -> int;
auto day24(int argc, char** argv) -> int;
auto day25(int argc, char** argv) -> int;

#endif
