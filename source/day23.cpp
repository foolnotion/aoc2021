#include "advent.hpp"
#include <fstream>
#include <vector>
#define XXH_INLINE_ALL
#include <xxhash.h>
#include <robin_hood.h>
#include <Eigen/Core>
using std::abs;

auto hash(auto const& arr) {
    return XXH_INLINE_XXH3_64bits(arr.data(), arr.size() * sizeof(char));
}

constexpr std::array hallidx { 0, 1, 3, 5, 7, 9, 10 };
constexpr std::array roomidx { 2, 4, 6, 8 };
constexpr std::array podcost { 1, 10, 100, 1000 };

auto day23(int argc, char **argv) -> int {
    if (argc < 2) {
        throw std::runtime_error("argument required");
    }

    std::ifstream f(argv[1]);
    std::string line;

    std::vector<std::string> lines;
    while(std::getline(f, line)) {
        lines.emplace_back(line);
    }
    Eigen::Array<char, -1, -1> arr(lines.size()-2, lines.front().size()-2);
    for(auto i = 1; i < std::ssize(lines)-1; ++i) {
        arr.row(i-1) = Eigen::Map<Eigen::Array<char, -1, 1>>(lines[i].data() + 1, lines[i].size() - 1);
    }
    auto roomsize = arr.rows() - 1;
    robin_hood::unordered_map<u64, i64> state;

    auto mincost{std::numeric_limits<u64>::max()};
    auto done = [&](auto const& arr) {
        return std::ranges::all_of(roomidx, [&](auto i) {
            return (arr.col(i).segment(1, roomsize) == static_cast<char>((i-2)/2 + 'A')).all();
        });
    };
    auto hall = arr.row(0);
    auto empty = [](char c) { return c == '.'; };

    auto search = [&](auto&& search, int cost) {
        auto h = hash(arr);
        if (auto [it, ok] = state.insert({h, cost}); !ok) {
            if (cost > it->second) { return; }
            it->second = cost;
        }

        if (done(arr)) {
            mincost = cost;
            return;
        }

        // move amphipod from hall to room
        for (auto i : hallidx) {
            auto c = hall(i);
            if (c == '.') { continue; }
            auto j = roomidx[c - 'A'];
            if (i < j && (hall.segment(i+1, j-i) != '.').any()) { continue; } // path blocked
            if (i > j && (hall.segment(j, i-j-1) != '.').any()) { continue; } // path blocked

            auto room = arr.col(j).segment(1, roomsize);
            if ((room == c || room == '.').all()) {
                auto it = std::ranges::partition_point(room, empty);
                auto r = std::distance(room.begin(), it);
                auto d = (r + abs(i - j)) * podcost[c - 'A'];
                if (cost + d >= mincost) { continue; }

                std::swap(hall(i), arr(r, j));
                search(search, cost + d);
                std::swap(hall(i), arr(r, j));
            }
        }

        // move amphipod from room to hall
        for (auto i : roomidx) {
            auto room = arr.col(i).segment(1L, roomsize);

            // check if anything needs to be moved
            auto it = std::ranges::partition_point(room, empty);
            if (it == room.end()) { continue; } // this room is empty
            if ((room == (i-2)/2+'A' || room == '.').all()) { continue; } // room is ok as is
            auto c = *it;
            auto r = std::distance(room.begin(), it) + 1;

            for (auto j : hallidx) {
                if (hall(j) != '.') { continue; } // this position is occupied by an amphipod
                if ((hall.segment(std::min(i, j), abs(i-j)) != '.').any()) { continue; } // path blocked
                auto d = (r + abs(i - j)) * podcost[c - 'A'];
                if (cost + d >= mincost) { continue; }

                std::swap(hall(j), arr(r, i));
                search(search, cost + d);
                std::swap(hall(j), arr(r, i));
            }
        }
    };
    search(search, 0UL);
    fmt::print("min cost: {}\n", mincost);
    return 0;
}
