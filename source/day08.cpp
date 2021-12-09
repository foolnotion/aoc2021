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

using std::array;
using std::ifstream;
using std::string;
using std::string_view;
using std::vector;
using std::views::iota;

constexpr u64 nseg = 7;    // number of segments
constexpr u64 ndigit = 10; // number of digits

constexpr array<char const*, ndigit> enc { "abcefg", "cf", "acdeg", "acdfg", "bcdf", "abdfg", "abdefg", "acf", "abcdefg", "abcdfg" };
constexpr inline auto bitconv(string_view s) -> u64 {
    u64 x{0};
    for (auto c : s) { x |= 1UL << static_cast<u64>(c - 'a'); }
    return x;
}

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

    // part1
    auto is_uniq = [](auto n) { return n == 2 || n == 3 || n == 4 || n == 7; }; // NOLINT
    auto part1 = std::transform_reduce(outputs.begin(), outputs.end(), 0UL, std::plus<>{}, [&](auto& out) {
        return std::count_if(out.begin(), out.end(), [&](auto const& s) { return is_uniq(s.size()); });
    });
    fmt::print("part 1: {}\n", part1);

    // part2
    auto validate = [&](auto const& chain, auto const& out) -> std::optional<u64> {
        u64 s{0};
        for (auto i : iota(0UL, out.size())) {
            u64 x{0};
            u64 v{0};
            for (auto c : out[i]) {
                x |= (1UL << chain[static_cast<u64>(c - 'a')]);
            }
            const auto *it = std::find(std::begin(bit), std::end(bit), x); 
            if (it == bit.end()) {
                return { };
            }
            auto p = std::distance(std::begin(bit), it) * std::pow(10, (out.size() - i - 1)); // NOLINT
            s += p; 
        }
        return { s };
    };

    vector<vector<u64>> size_groups {
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
                    seg[j] &= ~v; // this segment is not active in any of this group's digits
                } else if (s == 1) {
                    seg[j] &= v; // the digit is uniquely identified
                }
            }
        }

        array<bool, nseg> visited = { false };
        array<u64, nseg> map = {0};

        auto search = [&](int n, auto&& self) -> std::optional<u64> {
            if (n == nseg) { return validate(map, outs); }
            for (auto x = seg[n]; x != 0; x &= (x-1)) {
                auto z = __builtin_ctzl(x);
                if (visited[z]) { continue; }
                map[z] = n;
                visited[z] = true;
                auto res = self(n+1, self);
                if (res) { return res; }
                visited[z] = false;
            }
            return std::nullopt;
        };
        auto res = search(0, search);
        ENSURE(res);
        return res;
    };

    u64 sum{0};
    for (auto i : iota(0UL, signals.size())) {
        sum += decode(signals[i], outputs[i]).value();
    }
    fmt::print("part 2: {}\n", sum);
    return 0;
}
