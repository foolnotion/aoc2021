#include "advent.hpp"

#include <fstream>
#include <robin_hood.h>
#include <utility>
#include <vector>
#include <ranges>
#include <deque>

using std::deque;
using std::ifstream;
using std::pair;
using std::string;
using std::vector;
using std::ranges::views::iota;

template<typename Key>
using flat_set = robin_hood::unordered_flat_set<Key>;

template<typename Key, typename Value>
using flat_map = robin_hood::unordered_flat_map<Key, Value>;

auto day12(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;

    flat_map<string, i64> visited;
    flat_map<string, vector<string>> map;

    vector<string> tokens;
    while(std::getline(infile, line)) {
        tokens.clear();
        util::tokenize(line, '-', tokens);
        visited.insert({tokens[0], 1});
        visited.insert({tokens[1], 1});
        map[tokens[0]].emplace_back(tokens[1]);
        map[tokens[1]].emplace_back(tokens[0]);
    }
    auto vis = visited;

    flat_set<std::string> paths;
    auto small = [](auto const& l) { return l != "start" && l != "end" && std::islower(l[0]); };

    constexpr i64 max_visit = 2;
    auto find_path = [&](std::string const& l, std::string s, auto&& rec) {
        if (small(l)) {
            if (visited[l] == max_visit) {
                return;
            }
            ++visited[l];
        }

        s += l;

        if (l == "end") {
            paths.insert(s);
            return;
        }

        auto it = map.find(l);
        if (it == map.end()) {
            return;
        }

        for (auto const& d : it->second) {
            if (d == "start") { continue; }
            rec(d, s, rec);
        }

        if (small(l)) {
            --visited[l];
        }
    };

    find_path("start", "", find_path);
    fmt::print("part 1: {}\n", paths.size());

    paths.clear();
    for (auto const& [s, v] : visited) {
        if (!small(s)) {
            continue;
        }
        visited = vis;
        visited[s] = 0;
        find_path("start", "", find_path);
    }
    fmt::print("part 2: {}\n", paths.size());

    return 0;
}
