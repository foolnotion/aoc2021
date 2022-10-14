#include "advent.hpp"

#include <fstream>
#include <robin_hood.h>
#include <utility>

#define XXH_INLINE_ALL
#include <xxh3.h>

using std::ifstream;
using std::string;
using std::vector;

template<typename Key>
using set_t = robin_hood::unordered_set<Key>;

template<typename Key, typename Value>
using map_t = robin_hood::unordered_map<Key, Value>;

inline auto hash(std::string const& s) {
    return XXH_INLINE_XXH3_64bits(s.data(), s.size());
}

auto day12(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;

    map_t<u64, u64> visited;
    map_t<u64, vector<u64>> map;
    set_t<u64> small;

    u64 const start = hash("start");
    u64 const end = hash("end");

    vector<string> tokens;
    while(std::getline(infile, line)) {
        tokens.clear();
        util::tokenize(line, '-', tokens);
        
        auto h0 = hash(tokens[0]);
        auto h1 = hash(tokens[1]);
        
        visited.insert({h0, 1});
        visited.insert({h1, 1});
        
        map[h0].emplace_back(h1);
        map[h1].emplace_back(h0);

        if (h0 != start && h0 != end && std::islower(tokens[0].front())) { small.insert(h0); }
        if (h1 != start && h1 != end && std::islower(tokens[1].front())) { small.insert(h1); } 
    }
    auto vis = visited;

    set_t<u64> paths;

    constexpr i64 max_visit = 2;
    auto find_path = [&](u64 h, u64 s, auto&& find_path) {
        decltype(visited)::iterator it;
        auto sml = small.contains(h);
        if (sml) {
            it = visited.find(h);
            if (it->second == max_visit) { return; }
            ++it->second;
        }

        std::array a{ h, s };
        s = XXH_INLINE_XXH3_64bits(a.data(), a.size() * sizeof(u64));

        if (h == end) {
            paths.insert(s);
            return;
        }

        auto it1 = map.find(h);
        if (it1 == map.end()) {
            return;
        }

        for (auto const& d : it1->second) {
            if (d == start) { continue; }
            find_path(d, s, find_path);
        }

        if (sml) {
            --it->second;
        }
    };

    find_path(start, 0UL, find_path);
    fmt::print("part 1: {}\n", paths.size());

    paths.clear();
    for (auto const& [s, v] : visited) {
        if (small.contains(s)) {
            visited = vis;
            visited[s] = 0;
            find_path(start, 0UL, find_path);
        }
    }
    fmt::print("part 2: {}\n", paths.size());

    return 0;
}
