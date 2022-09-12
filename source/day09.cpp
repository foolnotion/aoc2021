#include "advent.hpp"

#include <bitset>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <ranges>
#include <scn/scn.h>
#include <unordered_set>
#include <vector>

#include <Eigen/Core>
#include <robin_hood.h>

using std::array;
using std::ifstream;
using std::pair;
using std::queue;
using std::string;
using std::string_view;
using std::vector;
using std::ranges::sort;
using std::views::iota;

auto day09(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;

    std::getline(infile, line);

    const i64 nrow = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n');
    const i64 ncol = std::ssize(line);

    Eigen::Array<i64, -1, -1> map(nrow + 1, ncol);

    infile.seekg(0); // rewind the input stream

    i64 row { 0 };
    while (std::getline(infile, line)) { // NOLINT
        //i64 v { 0 };
        for (auto i : iota(0, std::ssize(line))) {
            auto ret = scn::scan_value<i64>(std::string{line[i]});
            map(row, i) = ret.value();
        }
        ++row;
    }

    using point = pair<i64, i64>;

    // part1
    vector<point> low_points;
    for (auto i : iota(0, map.rows())) {
        auto x = std::max(0, i-1);
        auto sx = std::min(3L - (i == 0), map.rows() - x);

        for (auto j : iota(0, map.cols())) {
            auto y = std::max(0, j-1);
            auto sy = std::min(3L - (j == 0), map.cols() - y);
            if ((map.block(x, y, sx, sy) <= map(i, j)).count() == 1) {
                low_points.emplace_back(i, j);
            }
        }
    }
    auto part1 = std::transform_reduce(low_points.begin(), low_points.end(), u64 { 0 }, std::plus<> {}, [&](auto p) { return map(p.first, p.second); }) + low_points.size();
    fmt::print("part 1: {}\n", part1);

    // part2
    // we simply do a fill flood 
    vector<u64> basin_sizes;
    basin_sizes.reserve(low_points.size());
    decltype(map) visited = decltype(map)::Zero(map.rows(), map.cols());
    queue<point> q;

    auto visit = [&](auto i, auto j) {
        if (visited(i, j)) { return; }
        visited(i, j) = 1;
        q.push({ i, j });
    };

    const i64 max_height = 9;
    auto valid = [max_height](auto x, auto v) { return x > v && x < max_height; };

    for (auto [i, j] : low_points) {
        visit(i, j);

        u64 sz { 0 };
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            auto v = map(x, y);
            ++sz;

            if (x > 0 && valid(map(x-1, y), v))              { visit(x-1, y); }
            if (x < map.rows() - 1 && valid(map(x+1, y), v)) { visit(x+1, y); }
            if (y > 0 && valid(map(x, y-1), v))              { visit(x, y-1); }
            if (y < map.cols() - 1 && valid(map(x, y+1), v)) { visit(x, y+1); }
        }
        basin_sizes.push_back(sz);
    }
    sort(basin_sizes);
    auto part2 = std::reduce(std::rbegin(basin_sizes), std::rbegin(basin_sizes) + 3, u64 { 1 }, std::multiplies<> {});
    fmt::print("part 2: {}\n", part2);

    return 0;
}
