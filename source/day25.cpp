#include "advent.hpp"

#include <iostream>

#include <fstream>
#include <vector>
#include <Eigen/Core>

auto parse(auto const& lines) {
    Eigen::Array<char, -1, -1> map(std::ssize(lines), std::ssize(lines.front()));
    for (auto i = 0; i < std::ssize(lines); ++i) {
        map.row(i) = Eigen::Map<Eigen::Array<char, -1, 1> const>(lines[i].data(), lines[i].size());
    }
    return map;
}

auto day25(int argc, char** argv) -> int {
    if (argc < 2) {
        throw std::runtime_error("argument required");
    }

    std::ifstream f(argv[1]);
    std::vector<std::string> lines;
    for (std::string line; std::getline(f, line); ) { lines.push_back(line); }
    auto map = parse(lines);

    auto aux = map;
    auto nrow = map.rows();
    auto ncol = map.cols();

    for (auto i = 1; ; ++i) {
        aux.setConstant('.');
        u64 moved{0};

        // move east
        for (auto i = 0; i < nrow; ++i) {
            for (auto j = 0; j < ncol; ++j) {
                if (map(i, j) != '>') { continue; }
                auto k = (j+1) * (j+1 < ncol);
                auto x = map(i, k) == '.' ? k : j;
                aux(i, x) = '>';
                moved += x == k;
            }
        }

        // move south
        for (auto i = 0; i < nrow; ++i) {
            auto k = (i+1) * (i+1 < nrow);
            for (auto j = 0; j < ncol; ++j) {
                if (map(i, j) != 'v') { continue; }
                auto x = (map(k, j) != 'v' && aux(k, j) == '.') ? k : i;
                aux(x, j) = 'v';
                moved += x == k;
            }
        }

        if (moved == 0) {
            fmt::print("{}\n", i);
            break;
        }
        std::swap(map, aux);
    }

    return 0;
}
