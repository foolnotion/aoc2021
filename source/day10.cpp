#include "advent.hpp"

#include <deque>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <scn/scn.h>
#include <vector>

using std::array;
using std::deque;
using std::ifstream;
using std::pair;
using std::string;
using std::string_view;
using std::vector;
using std::ranges::sort;
using std::views::iota;
using std::views::reverse;

constexpr array<char, 8> delimiters = { '(', '[', '{', '<', ')', ']', '}', '>' };

constexpr array<i64, 4> error_score = {
        3, // )
       57, // ]
     1197, // }
    25137  // >
};

constexpr array<i64, 4> completion_score = {
    1, // )
    2, // ]
    3, // }
    4  // >
};

auto delimiter_index(auto c) {
    auto const *begin = std::begin(delimiters);
    return std::distance(begin, std::find(begin, std::end(delimiters), c));
}

template<auto... Ts> auto is(auto c) -> bool { return ((c == Ts ) || ...); }
auto is_left(char c) { return is<'(', '[', '{', '<'>(c); } 
auto is_right(char c) { return is<')', ']', '}', '>'>(c); }

auto match(char a, char b) -> bool {
    return
        (a == '(' && b == ')') ||
        (a == '[' && b == ']') ||
        (a == '{' && b == '}') ||
        (a == '<' && b == '>');
}

constexpr u64 score_multiplier = 5;
auto get_completion_score(auto const& s) -> u64
{
    u64 score{0};
    for (auto c : reverse(s)) {
        score *= score_multiplier;
        score += completion_score[delimiter_index(c)];
    }
    return score;
}

auto day10(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }
    deque<char> s;
    u64 part1{0};
    vector<u64> scores;

    ifstream infile(argv[1]); // NOLINT
    string line;
    while(std::getline(infile, line)) {
        s.clear();
        bool err{false};
        for (auto c : line) {
            if (is_left(c)) {
                s.push_back(c);
            }
            if (is_right(c)) {
                auto d = s.back();
                s.pop_back();
                if(err = !match(d, c); err) {
                    part1 += error_score[delimiter_index(c)-4];
                    break;
                }
            }
        }
        if (!err) {
            scores.push_back(get_completion_score(s));
        }
    }
    fmt::print("part 1: {}\n", part1);
    sort(scores);
    fmt::print("part 2: {}\n", scores[scores.size()/2]);

    return 0;
}
