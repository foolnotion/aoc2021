#include "advent.hpp"

#include <cstdint>
#include <fstream>
#include <gsl/gsl_util>
#include <iostream>
#include <scn/scn.h>
#include <Eigen/Dense>

auto day05(int argc, char** argv) -> int
{
    std::ifstream infile(argv[1]); // NOLINT
    std::string str;

    using point = std::tuple<int64_t, int64_t>;
    using line = std::pair<point, point>;
    std::vector<line> lines;

    while(std::getline(infile, str)) {
        std::vector<std::string> tokens;
        util::tokenize(str, ' ', tokens);
        int x = 0;
        int y = 0;
        scn::scan(tokens[0], "{},{}", x, y);
        point p1{x, y};
        scn::scan(tokens[2], "{},{}", x, y);
        point p2{x, y};
        lines.emplace_back(p1, p2);
    }

    int64_t xmax = std::numeric_limits<int64_t>::min();
    int64_t ymax = xmax;

    for(auto [p1, p2] : lines) {
        auto [x1, y1] = p1;
        auto [x2, y2] = p2;
        xmax = std::max({xmax, x1, x2});
        ymax = std::max({ymax, y1, y2});
    }

    Eigen::Array<int, -1, -1> map = decltype(map)::Zero(xmax + 1, ymax + 1);
    for (auto [p1, p2] : lines) {
        auto [x1, y1] = p1;
        auto [x2, y2] = p2;

        if (x1 == x2) { // horizontal
            map(x1, Eigen::seq(std::min(y1, y2), std::max(y1, y2))) += 1;
        } else if (y1 == y2) { // vertical
            map(Eigen::seq(std::min(x1, x2), std::max(x1, x2)), y1) += 1;
        } else { // diagonal
            auto xx = std::abs(x1-x2);
            auto yy = std::abs(y1-y2);
            if (xx == yy) {
                Eigen::Array<int, -1, -1> block = decltype(block)::Zero(xx+1, yy+1); 
                block.matrix().diagonal().array() += 1;

                if (x1 > x2 && y1 < y2) {
                    map.block(x2, y1, xx+1, yy+1) += block.rowwise().reverse();
                } else if (x1 < x2 && y1 > y2) {
                    map.block(x1, y2, xx+1, yy+1) += block.colwise().reverse();
                } else {
                    map.block(std::min(x1, x2), std::min(y1, y2), xx+1, yy+1) += block;
                }
            }
        }
    }
    auto count = (map > 1).count();
    fmt::print("count: {}\n", count);
    
    return 0;
}
