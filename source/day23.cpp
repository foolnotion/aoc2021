#include "advent.hpp"

#include <fstream>
#include <ostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>
#include <robin_hood.h>

using std::abs;

constexpr std::array hallpos { 0L, 1L, 3L, 5L, 7L, 9L, 10L };
constexpr std::array roompos { 2L, 4L, 6L, 8L };
constexpr std::array podcost { 1UL, 10UL, 100UL, 1000UL };
constexpr std::array symbols { 'A', 'B', 'C', 'D', '.' };

enum kind { a, b, c, d, empty };

constexpr u8 n_state{3}; // 3 bits for state (A=000, B=001, C=010, D=011, empty=100)
constexpr u8 n_hall{33}; // 33 used bits (11 x 3)
constexpr u8 state_mask{0x7U}; // state mask

constexpr u16 A{ 0x000U }; // good state for room a NOLINT
constexpr u16 B{ 0x249U }; // good state for room b NOLINT
constexpr u16 C{ 0x492U }; // good state for room b NOLINT
constexpr u16 D{ 0x6DBU }; // good state for room b NOLINT
constexpr u16 E{ 0x924U }; // empty room state NOLINT
                           //
constexpr u64 hall_init{0x124924924U}; // empty hall
constexpr u64 hall_mask{static_cast<u64>(-1) >> static_cast<u64>(std::numeric_limits<u64>::digits - n_hall)};
constexpr std::array done { A, B, C, D };

template<std::size_t S /* room capacity (2 or 4 for this puzzle) */>
struct state_t { // NOLINT
    u64 hall{hall_init}; // we use 33 out of 64 bits, order from right to left
    std::array<uint16_t, roompos.size()> rooms{A, B, C, D}; // we use 12 out of 16 bits per room, order from right to left
    u64 cost{0UL};
    u64 hcost{0UL};

    static constexpr std::size_t roomsize = S;

    [[nodiscard]] auto heuristic() const {
        u64 total = 0;
        for (auto i : hallpos) {
            auto x = get(i);
            if (x == kind::empty) { continue; }
            if (i < roompos[x] && !path_clear({i+1, roompos[x]})) {
                return std::numeric_limits<u64>::max();
            }
            if (i > roompos[x] && !path_clear({roompos[x], i-1})) {
                return std::numeric_limits<u64>::max();
            }
            total += abs(i - roompos[x]) * podcost[x];
        }
        for (auto i = 0; i < std::ssize(rooms); ++i) {
            for (auto j = 0; j < S; ++j) {
                auto x = get(i, j);
                if (x == kind::empty) { continue; }
                if (roompos[i] == roompos[x]) { continue; }
                total += (abs(roompos[i] - roompos[x])) * podcost[x];
                if (!path_clear({roompos[i], roompos[x]})) {
                    return std::numeric_limits<u64>::max();
                }
                total += (1 + j + abs(roompos[i] - roompos[x])) * podcost[x];
            }
        }
        return total;
    };

    // set room position to value val
    auto set(u32 idx, u32 pos, u8 val) { // NOLINT
        u16 r = rooms[idx];
        u16 b = pos * n_state; // shift width in bits
        u16 ones = static_cast<u16>(-1);
        u16 m = ones << static_cast<u16>(b + n_state) | ~(ones << b);
        u16 v = static_cast<u16>(val) << b;
        rooms[idx] = (r & m) | v; // NOLINT
    }

    // set hall position to value val
    auto set(u32 pos, u8 val) { // NOLINT
        u64 b = static_cast<u64>(pos) * n_state;
        u64 m = static_cast<u64>(-1) << (b + n_state) | ~(static_cast<u64>(-1) << b);
        u64 v = static_cast<u64>(val) << b;
        hall = (hall & m) | v;
    }

    // enter room, return number of steps taken inside
    auto enter(u32 idx) {
        assert(can_enter(idx));
        auto i = S-1;
        while (get(idx, i) != kind::empty && i >= 0) { --i; }
        set(idx, i, idx);
        return i+1;
    }

    // exit room, returning the amphi type and number of steps taken
    auto exit(u32 idx) -> std::pair<u8, u32> {
        assert(rooms[idx] != E);
        auto i = 0;
        while (get(idx, i) == kind::empty && i < S) { ++i; }
        auto x = get(idx, i);
        set(idx, i, kind::empty);
        return { x, i+1 };
    }

    // return room value at pos
    [[nodiscard]] auto get(u32 idx, u32 pos) const {
        auto v = static_cast<u8>((rooms[idx] >> (pos * n_state)) & state_mask);
        return v;
    }

    // return hall value at pos
    [[nodiscard]] auto get(u32 pos) const {
        return static_cast<u8>((hall >> (pos * n_state)) & state_mask);
    }

    // can this room be entered by the correct amphi?
    [[nodiscard]] auto good(u32 idx) const {
        for (auto i = 0; i < S; ++i) {
            auto x = get(idx, i);
            if (!(x == idx || x == kind::empty)) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] auto empty(u32 idx) const { return rooms[idx] == E; }

    // check if the path is clear in this range (inclusive at both ends)
    [[nodiscard]] auto path_clear(std::pair<u32, u32> range) const -> bool {
        auto [from, to] = range;
        if (from > to) { return path_clear({ to, from }); }
        for (auto i = from; i <= to; ++i) {
            if (get(i) != kind::empty) { return false; }
        }
        return true;
    }

    // return true if all the amphipods are arranged
    explicit operator bool() const {
        return std::ranges::equal(rooms, done);
    }

    friend auto operator<(state_t a, state_t b) {
        return a.cost + a.hcost > b.cost + b.hcost;
    }

    friend auto operator==(state_t a, state_t b) {
        return a.hall == b.hall && std::ranges::equal(a.rooms, b.rooms);
    }
};

template<typename S>
struct hash {
    auto operator()(S s) const -> u64 {
        u64 r{0};
        std::memcpy(&r, s.rooms.data(), s.rooms.size() * sizeof(u16));
        return r * 31 + s.hall; // NOLINT
    }
};

template<typename S>
auto search(S start) -> u64 {
    std::priority_queue<S, std::vector<S>, std::less<>> q;
    robin_hood::unordered_set<S, hash<S>> map;
    u64 mincost{std::numeric_limits<u64>::max()};

    q.push(start);
    while(!q.empty()) {
        auto const s = q.top(); q.pop();
        auto cost = s.cost + s.hcost;
        if(s) { mincost = std::min(s.cost, mincost); continue; }
        if (cost >= mincost) { continue; }
        if (auto [it, ok] = map.insert(s); !ok) { continue; }

        // iterate hall positions and check if amphi can move to room
        for (auto i : hallpos) {
            auto x = s.get(i);
            if (x == kind::empty) { continue; }
            if (i < roompos[x] && !s.path_clear({i+1, roompos[x]})) { continue; }
            if (i > roompos[x] && !s.path_clear({roompos[x], i-1})) { break; }
            if (!s.good(x)) { continue; }
            auto p = s;
            auto distance = abs(i - roompos[x]) + p.enter(x);
            p.set(i, kind::empty); // remove from hall
            p.cost += distance * podcost[x];
            p.hcost = p.heuristic();
            q.push(p);
        }

        // iterate rooms and check if amphi can move to hall
        for (auto i = 0; i < std::ssize(s.rooms); ++i) {
            if (s.empty(i) || s.good(i)) { continue; } // this room is good, no need to do anything
            for (auto k : hallpos) {
                if (!s.path_clear({roompos[i], k})) {
                    if (roompos[i] < k) { break; }
                    continue;
                }
                auto p = s;
                auto [x, t] = p.exit(i);
                auto distance = abs(roompos[i] - k) + t;
                p.set(k, x); // move to halll
                p.cost += distance * podcost[x];
                p.hcost = p.heuristic();
                q.push(p);
            }
        }
    }
    return mincost;
};

template<typename S, std::ranges::random_access_range R>
auto parse(R lines) {
    S start;
    for(auto i = 1; i < std::ssize(lines)-1; ++i) {
        for (auto j = 0; j < std::size(lines[i]); ++j) {
            auto c = lines[i][j];
            if (std::isalpha(c)) {
                start.set((j-3)/2, i-2, c-'A');
            }
        }
    }
    return start;
}

auto day23(int argc, char **argv) -> int {
    std::vector<std::string> lines;

    std::ifstream part1("./input/23_part1.txt");
    for (std::string line; std::getline(part1, line); lines.emplace_back(line));
    auto s1 = parse<state_t<2>>(lines);

    lines.clear();
    std::ifstream part2("./input/23_part2.txt");
    for (std::string line; std::getline(part2, line); lines.emplace_back(line));
    auto s2 = parse<state_t<4>>(lines);

    fmt::print("part 1: {}\n", search(s1));
    fmt::print("part 2: {}\n", search(s2));
    return 0;
}
