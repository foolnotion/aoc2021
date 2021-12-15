#include "advent.hpp"

#include <Eigen/Core>
#include <concepts>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using Eigen::Array;

using std::ifstream;
using std::priority_queue;
using std::string;
using std::vector;

template<typename T, typename U, typename Cmp = std::less<U>>
requires std::equality_comparable<U>
struct priority_item {
    T data;
    U priority;

    friend auto operator<(priority_item const& lhs, priority_item const& rhs) -> bool {
        return Cmp{}(lhs.priority, rhs.priority); 
    }

    friend auto operator==(priority_item const& lhs, priority_item const& rhs) -> bool {
        return lhs.priority == rhs.priority;
    }
};

struct point {
    i64 x;
    i64 y;
    auto operator<=>(const point&) const -> bool = default;
};

auto day15(int argc, char** argv) -> int
{
    // read input
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;
    std::getline(infile, line);

    const i64 ncol = line.size();
    const i64 nrow = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n') + 1;
    infile.seekg(0); // rewind the input stream
    Array<i64, -1, -1> map(nrow, ncol);

    i64 row{0};
    while(std::getline(infile, line)) {
        std::string str(line);
        std::transform(str.begin(), str.end(), map.row(row).begin(), [](auto c) { return static_cast<i64>(c - '0'); });
        ++row;
    }

    // define a simple A* search
    using item = priority_item<point, i64, std::greater<>>;
    decltype(map) costs = map;

    auto astar = [&](auto start, auto end, auto&& cost) {
        priority_queue<item> q; 
        q.push({start, 0L});

        costs.fill(-1);
        costs(start.x, start.y) = cost(start);

        while (!q.empty()) {
            auto [current, prio] = q.top(); q.pop();
            if (current == end) {
                break;
            }
            auto [x, y] = current;
            auto c = costs(x, y);
            for (auto i = std::max(0L, x-1); i <= std::min(end.x, x+1); ++i) {
                for (auto j = std::max(0L, y-1); j <= std::min(end.y, y+1); ++j) {
                    if ((i == x && j == y) || (i != x && j != y)) {
                        continue;
                    }
                    point next{i, j};
                    auto new_cost = c + cost(next);
                    if (costs(i, j) == -1 || new_cost < costs(i, j)) {
                        costs(i, j) = new_cost;
                        q.push({next, new_cost});
                    }
                }
            }
        }
    };

    // part 1
    auto cost_p1 = [&](point const& p) { return map(p.x, p.y); };
    const point s1{0, 0};
    const point s2{map.rows()-1, map.cols()-1};
    astar(s1, s2, cost_p1);
    fmt::print("part 1: {}\n", costs(s2.x, s2.y) - costs(s1.x, s1.y));

    // part 2
    constexpr i64 times_larger{5};
    constexpr i64 wrap{9};

    auto cost_p2 = [&](point const& p) {
        auto d = p.x / map.rows() + p.y / map.cols();
        auto x = p.x % map.rows();
        auto y = p.y % map.cols();
        auto c = map(x, y) + d;
        return c > wrap ? c - wrap : c;
    };
    costs.resize(map.rows() * times_larger, map.cols() * times_larger);
    const point s3{costs.rows()-1, costs.cols()-1};
    astar(s1, s3, cost_p2);
    fmt::print("part 2: {}\n", costs(s3.x, s3.y) - costs(s1.x, s1.y));

    return 0;
}
