#include "advent.hpp"

#include <iostream>
#include <fstream>
#include <robin_hood.h>
#include <vector>
#include <string>
#include <fmt/ranges.h>

#include <Eigen/Core>

using std::pair;
using std::ifstream;
using std::string;
using std::vector;

template<typename K, typename V>
using flat_map = robin_hood::unordered_flat_map<K, V>;

template<typename K>
using flat_set = robin_hood::unordered_flat_set<K>;

auto day14(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;

    flat_map<std::string, char> rules;

    std::getline(infile, line);
    auto t = line; // the polymer template
    
    vector<string> tokens;
    while (std::getline(infile, line)) {
        if (line.empty()) { continue; }
        tokens.clear();
        util::tokenize(line, ' ', tokens);
        ENSURE(!tokens[0].empty());
        rules[tokens[0]] = tokens[2][0];
    }

    constexpr i64 alphabet_size = 26;
    auto idx = [](char c) { return static_cast<i64>(std::tolower(c) - 'a'); };
    constexpr i64 steps{40};

    Eigen::Array<i64, alphabet_size, alphabet_size> cnt;
    cnt.fill(0);
    for (auto i = 0UL; i < std::ssize(t) - 1; ++i) {
        auto a = idx(t[i]);
        auto b = idx(t[i+1]);
        ++cnt(a, b);
    }

    for (i64 step = 0; step < steps; ++step) {
        auto tmp = cnt;
        for (auto const& [k, v] : rules) {
            auto a = idx(k[0]);
            auto b = idx(k[1]);
                if (cnt(a, b) > 0) {
                auto c = idx(rules[k]);
                tmp(a, c) += cnt(a, b);
                tmp(c, b) += cnt(a, b);
                tmp(a, b) -= cnt(a, b);
            }
        }
        std::swap(cnt, tmp);
    }
    i64 min_count{std::numeric_limits<i64>::max()};
    i64 max_count{0};
    for (i64 i = 0; i != cnt.rows(); ++i) {
        auto row = cnt.row(i);
        auto sum = row.sum();
        if (sum > 0) {
            min_count = std::min(min_count, sum);
            max_count = std::max(max_count, sum);
        }
    }
    fmt::print("min: {}, max: {}\n", min_count, max_count);
    fmt::print("part 2: {}\n", max_count - min_count + 1);

    return 0;
}
