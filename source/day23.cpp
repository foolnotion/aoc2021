#include "advent.hpp"

#include <fstream>
#include <vector>

#define XXH_INLINE_ALL
#include <xxhash.h>

#include <robin_hood.h>

#include <Eigen/Core>

using fmt::print;
using std::vector;
using std::abs;

auto hash(auto const& arr) {
    return XXH_INLINE_XXH3_64bits(arr.data(), arr.size() * sizeof(char));
}

constexpr int hallrow{1};
constexpr std::array hallidx { 1, 2, 4, 6, 8, 10, 11 };
constexpr std::array roomidx { 3, 5, 7, 9 };
constexpr std::array podcost { 1, 10, 100, 1000 };

auto day23(int argc, char **argv) -> int {
    if (argc < 2) {
        throw std::runtime_error("argument required");
    }

    std::ifstream f(argv[1]);
    std::string line;


    vector<std::string> lines;
    while(std::getline(f, line)) {
        lines.emplace_back(line);
    }
    Eigen::Array<char, -1, -1, Eigen::RowMajor> arr(lines.size(), lines.front().size());
    for(auto i = 0; i < std::ssize(lines); ++i) {
        arr.row(i) = Eigen::Map<Eigen::Array<char, -1, 1>>(lines[i].data(), lines[i].size());
    }
    auto roomsize = arr.rows() - 3;
    robin_hood::unordered_map<u64, i64> state;

    auto mincost{std::numeric_limits<u64>::max()};
    auto done = [&](auto const& arr) {
        return std::ranges::all_of(roomidx, [&](auto i) {
            return (arr.col(i).segment(hallrow+1, roomsize) == static_cast<char>((i-3)/2 + 'A')).all();
        });
    };
    auto hall = arr.row(hallrow);
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
            auto c = arr(hallrow, i);
            if (c == '.') { continue; }

            auto j = roomidx[c - 'A'];

            if (i < j && (hall.segment(i+1, j-i) != '.').any()) { continue; } // path blocked
            if (i > j && (hall.segment(j, i-j-1) != '.').any()) { continue; } // path blocked

            auto row = hallrow + 1;
            auto room = arr.col(j).segment(hallrow+1, roomsize);
            auto can_enter = (room == c || room == '.').all();
            if (can_enter) {
                auto it = std::ranges::partition_point(room.begin(), room.end(), empty);
                auto r = std::distance(room.begin(), it) + hallrow;
                auto d = (abs(r - hallrow) + abs(i - j)) * podcost[c - 'A'];

                if (cost + d < mincost) {
                    std::swap(arr(hallrow, i), arr(r, j));
                    search(search, cost + d);
                    std::swap(arr(hallrow, i), arr(r, j));
                }
            }
        }

        // move amphipod from room to hall
        for (auto i : roomidx) {
            auto room = arr.col(i).segment(hallrow+1L, roomsize);

            // check if anything needs to be moved
            auto it = std::ranges::partition_point(room, empty);
            if (it == room.end()) { continue; } // this room is empty
            if ((room == (i-3)/2+'A' || room == '.').all()) {
                continue;
            }
            auto c = *it;
            auto r = hallrow + std::distance(room.begin(), it) + 1;

            for (auto j : hallidx) {
                if (arr(hallrow, j) != '.') { continue; } // this position is occupied by an amphipod
                if (i < j && (hall.segment(i, j-i) != '.').any()) { continue; } // path blocked
                if (i > j && (hall.segment(j, i-j) != '.').any()) { continue; } // path blocked
                auto d = (abs(r - hallrow) + abs(i - j)) * podcost[c - 'A'];

                if (cost + d < mincost) {
                    std::swap(arr(hallrow, j), arr(r, i));
                    search(search, cost + d);
                    std::swap(arr(hallrow, j), arr(r, i));
                }
            }
        }
    };
    search(search, 0UL);
    fmt::print("min cost: {}\n", mincost);
    return 0;
}
