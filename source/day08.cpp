#include "advent.hpp"

#include <bitset>
#include <fmt/ranges.h>
#include <fstream>
#include <gsl/gsl_util>
#include <iostream>
#include <numeric>
#include <ranges>
#include <scn/scn.h>
#include <vector>
#include <stack>

using std::array;
using std::ifstream;
using std::stack;
using std::string;
using std::string_view;
using std::vector;
using std::views::iota;

constexpr u64 nseg = 7;
constexpr u64 ndigit = 10;

constexpr array<i64, ndigit> digit_nseg { 6, 2, 5, 5, 4, 5, 6, 3, 7, 6 }; // digit segments
constexpr array<char const*, ndigit> enc { "abcefg", "cf", "acdeg", "acdfg", "bcdf", "abdfg", "abdefg", "acf", "abcdefg", "abcdfg" };

constexpr inline auto bitconv(string_view s) -> u64 {
    u64 x{0};
    for (auto c : s) { x |= 1UL << static_cast<u64>(c - 'a'); }
    return x;
}

constexpr inline auto trz(auto x) { return __builtin_ctzl(x); }
constexpr inline auto popcnt(auto x) { return __builtin_popcount(x); }

constexpr array<u64, ndigit> bit {
    bitconv(enc[0]),
    bitconv(enc[1]),
    bitconv(enc[2]),
    bitconv(enc[3]),
    bitconv(enc[4]),
    bitconv(enc[5]),
    bitconv(enc[6]),
    bitconv(enc[7]),
    bitconv(enc[8]),
    bitconv(enc[9])
};

auto day08(int argc, char** argv) -> int
{
    ENSURE(bit[0] == 0b1110111);
    ENSURE(bit[1] == 0b0100100);
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string str;

    vector<string> tokens;
    string line;

    vector<vector<string>> signals;
    vector<vector<string>> outputs;

    vector<string> tmp;
    while (std::getline(infile, line)) {
        tokens.clear();
        util::tokenize(line, '|', tokens);
        tmp.clear();
        scn::scan_list(tokens[0], tmp, ' ');
        signals.emplace_back(tmp.begin(), tmp.end());
        tmp.clear();
        scn::scan_list(tokens[1], tmp, ' ');
        outputs.emplace_back(tmp.begin(), tmp.end());
    }

    ENSURE(signals.size() == outputs.size()); 

    auto is_uniq = [](auto n) { return n == digit_nseg[1] || n == digit_nseg[4] || n == digit_nseg[7] || n == digit_nseg[8]; }; // NOLINT

    // part1
    auto part1 = std::transform_reduce(outputs.begin(), outputs.end(), 0UL, std::plus<>{}, [&](auto& out) {
        return std::count_if(out.begin(), out.end(), [&](auto const& s) { return is_uniq(s.size()); });
    });
    fmt::print("part 1: {}\n", part1);

    // part2
    vector<u64> vec(enc.size());

    auto size_to_digit = [](auto sz) {
        if (sz == 2) { return 1; } // NOLINT
        if (sz == 3) { return 7; } // NOLINT
        if (sz == 4) { return 4; } // NOLINT
        if (sz == 7) { return 8; } // NOLINT
        return 0;
    };

    auto pred = [](auto const& a, auto const& b) { return a.first == b.first; };
    auto to_char = [](auto v) { return static_cast<char>('a' + v); };

    auto validate = [&](auto const& chain, auto const& out) -> std::optional<u64> {
        u64 s{0};
        u64 j{0};
        for (auto const& str : out) {
            u64 x{0};
            u64 v{0};
            for (auto c : str) {
                x |= (1UL << chain[static_cast<u64>(c - 'a')]);
            }
            const auto *it = std::find(std::begin(bit), std::end(bit), x); 
            if (it == bit.end()) {
                return { };
            }
            auto p = std::distance(std::begin(bit), it) * std::pow(10, (out.size() - j - 1));
            s += p; 
            ++j;
        }
        return { s };
    };

    vector<vector<u64>> size_groups = {
        {}, {}, {1}, {7}, {4}, {2, 3, 5}, {0, 6, 9}, {8} // NOLINT
    };

    auto decode = [&](auto const& signals, auto const& outs) {
        vector<std::pair<u64, u64>> values;
        for (auto const& s : signals) {
            values.emplace_back(bitconv(s), s.size());
        }
        vector<u64> seg(nseg, bit[8]); // NOLINT

        for (auto i : iota(0UL, values.size())) {
            auto [v, s] = values[i];
            auto const& g = size_groups[s];

            for(auto j : iota(0UL, nseg)) {
                if (std::none_of(g.begin(), g.end(), [&](auto x) { return bit[x] & (1UL << j); })) {
                    // this segment is not active in any of this group's digits,
                    // therefore it cannot be mapped to any of the letters describing this group
                    seg[j] &= ~v;
                } else if (s == 1) {
                    // the digit is uniquely identified
                    seg[j] &= v;
                }
            }
        }

        array<bool, nseg> visited = { false };
        array<u64, nseg> map = {0};

        auto search = [&](int n, auto&& self) -> std::optional<u64> {
            if (n == nseg) { return validate(map, outs); }
            for (auto x = seg[n]; x != 0; x &= (x-1)) {
                auto z = trz(x);
                if (visited[z]) { continue; }
                map[z] = n;
                visited[z] = true;
                auto res = self(n+1, self);
                if (res) { return res; }
                visited[z] = false;
            }
            return std::nullopt;
        };

        return search(0, search);
    };

    u64 sum{0};
    for (auto i : iota(0UL, signals.size())) {
        sum += decode(signals[i], outputs[i]).value();
    }
    fmt::print("part 2: {}\n", sum);
    return 0;
}
