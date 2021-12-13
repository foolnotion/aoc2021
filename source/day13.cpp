#include "advent.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <scn/scn.h>
#include <tuple>

#include <Eigen/Core>

using std::pair;
using std::ifstream;
using std::string;
using std::vector;
using Eigen::Array;


auto day13(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;

    i64 x{0};
    i64 y{0};
    vector<pair<i64, i64>> points;
    while (std::getline(infile, line)) { // NOLINT
        if (line.empty()) { break; }
        scn::scan(std::string(line), "{},{}", y, x);
        points.emplace_back(x, y);
    }
    for (auto [a, b] : points) {
        x = std::max(x, a);
        y = std::max(y, b);
    }
    Array<i64, -1, -1> paper = decltype(paper)::Zero(x+1, y+1);
    vector<pair<char, i64>> folds;

    vector<string> tokens;
    while(std::getline(infile, line)) { // NOLINT 
        tokens.clear();
        util::tokenize(line, '=', tokens);
        folds.emplace_back(tokens[0].back(), std::stoi(tokens[1]));
    }

    for (auto [a, b] : points) {
        paper(a, b) = 1;
    }

    using Eigen::all;
    using Eigen::seqN;

    x = paper.rows();
    y = paper.cols();

    i64 part1{-1};
    for (auto [c, v] : folds) {
        auto b = paper.block(0, 0, x, y);
        if (c == 'x') {
            b(all, seqN(0, v)) += b(all, seqN(v+1, b.cols()-v-1)).rowwise().reverse();
            y = v;
        }
        if (c == 'y') {
            b(seqN(0, v), all) += b(seqN(v+1, b.rows()-v-1), all).colwise().reverse();
            x = v;
        }
        if (part1 == -1) { part1 = (paper.block(0, 0, x, y) > 0).count(); }
    }
    auto b = paper.block(0, 0, x, y);
    b = (b > 1).select(1, b);
    for (i64 i = 0; i < x; ++i) { // NOLINT
        for (i64 j = 0; j < y; ++j) { // NOLINT
            auto c = b(i, j) == 1 ? '#' : ' ';
            fmt::print("{} ", c);
        }
        fmt::print("\n");
    }
    fmt::print("part 1: {}\n", part1);

    return 0;
}
