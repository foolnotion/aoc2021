#include "advent.hpp"

#include <Eigen/Core>
#include <algorithm>
#include <array>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <scn/scan/list.h>
#include <scn/scn.h>
#include <vector>

using std::vector;
using std::ranges::sort;
using std::views::iota;
using std::views::transform;

auto day07(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    std::ifstream infile(argv[1]); // NOLINT
    std::string str;

    std::getline(infile, str);
    vector<i64> pos; // positions
    auto res = scn::scan_list_ex(str, pos, scn::list_separator(','));
    sort(pos);
    auto const sz = std::ssize(pos);
    auto med = sz % 2 == 0 ? pos[sz / 2 - 1] : static_cast<i64>(std::midpoint(pos[sz / 2 - 1], pos[sz / 2]));
    auto mean = std::accumulate(pos.begin(), pos.end(), double { 0 }) / static_cast<double>(sz);
    fmt::print("mean: {}, median: {}\n", mean, med);

    // part 1
    auto costs1 = pos | transform([&](auto p) { return std::abs(med - p); });
    fmt::print("part 1: {} (pos = {})\n", std::accumulate(costs1.begin(), costs1.end(), i64 { 0 }), med);

    // part 2
    auto sum = [](auto n) { return n * (n + 1) / 2; };

    i64 min_cost = std::numeric_limits<i64>::max();
    i64 min_pos = 0;
    for (auto i : iota(static_cast<i64>(std::floor(mean-1)), static_cast<i64>(std::ceil(mean+1)))) {
        auto costs2 = pos | transform([&](auto p) { return sum(std::abs(i - p)); });
        auto cost = std::accumulate(costs2.begin(), costs2.end(), i64 { 0 });
        if (min_cost > cost) {
            min_cost = cost;
            min_pos = i;
        }
    }
    fmt::print("part 2: {} (pos = {})\n", min_cost, min_pos);

    return 0;
}
