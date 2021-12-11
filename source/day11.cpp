#include "advent.hpp"

#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fstream>
#include <gsl/gsl_util>
#include <iostream>
#include <tuple>
#include <ranges>
#include <scn/scn.h>
#include <vector>

#include <Eigen/Core>

using std::ifstream;
using std::pair;
using std::string;
using std::vector;
using std::ranges::views::iota;

using Eigen::Array;

constexpr i64 energy_level = 9;

auto get_block(auto& m, auto i, auto j) -> std::tuple<i64, i64, i64, i64> {
    auto x = std::max(0L, i-1);
    auto y = std::max(0L, j-1);
    return std::make_tuple(x, y,
            std::min(3L - (i == 0), m.rows() - x),
            std::min(3L - (j == 0), m.cols() - y));
}

auto flash(auto& m, auto& f, auto i, auto j) {
    if (f(i, j) || m(i, j) <= energy_level) { return; }
    f(i, j) = 1;
    auto [x, y, w, h] = get_block(m, i, j);
    m.block(x, y, w, h) += 1;
    for (auto ii = x; ii < x + w; ++ii) {
        for (auto jj = y; jj < y + h; ++jj) {
            flash(m, f, ii, jj);
        }
    }
};

auto day11(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;
    std::getline(infile, line);
    const i64 ncol = std::ssize(line);
    const i64 nrow = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n');
    infile.seekg(0); // rewind the input stream

    Array<i64, -1, -1> map(nrow + 1, ncol);
    i64 row{0};
    while (std::getline(infile, line)) { // NOLINT
        i64 col{0};
        for (auto c : line) {
            map(row, col++) = static_cast<int>(c - '0');
        }
        ++row;
    }

    decltype(map) flashed = decltype(map)::Zero(map.rows(), map.cols());

    i64 part1{0};
    i64 part2{0};
    const i64 part1_steps{100};

    for (auto s = 1UL; ; ++s) {
        flashed.fill(0);
        map += 1;

        for (auto i : iota(0L, map.rows())) {
            for (auto j : iota(0L, map.cols())) {
                flash(map, flashed, i, j);
            }
        }
        map = (map > energy_level).select(0, map);
        auto flashes = (map == 0).count();
        if (s <= part1_steps) {
            part1 += flashes;
        }
        if (flashes == map.size()) {
            part2 = s;
            break;
        }
    }
    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);
    
    return 0;
}
