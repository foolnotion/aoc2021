#include "advent.hpp"
#include <ranges>
#include <robin_hood.h>
#include <xxhash.h>

struct dice {
    static constexpr u64 dmax { 100 };
    auto operator()()
    {
        ++count;
        return value++ % dmax + 1;
    }

    auto operator()(u64 times)
    {
        auto v { 0UL };
        for (auto i = 0UL; i < times; ++i) {
            v += (*this)();
        }
        return v;
    }

    u64 value { 0 };
    u64 count { 0 };
};

struct player {
    u64 position {};
    u64 score {};

    static constexpr u64 maxpos { 10 };

    auto update(u64 v)
    {
        position = (position + v) % maxpos;
        score += position + 1;
        return score;
    }
};

template <size_t S /* max score */>
struct game {
    std::array<player, 2> players;
    bool turn { false };

    auto update(u64 v) -> bool
    {
        turn = !turn;
        return players[!turn].update(v) >= S;
    }

    [[nodiscard]] auto hash() const -> u64
    {
        auto const& [p1, p2] = players;
        std::array arr { p1.position, p1.score, p2.position, p2.score, (u64)turn };
        return XXH3_64bits(arr.data(), arr.size() * sizeof(decltype(arr)::value_type));
    }

    [[nodiscard]] auto winner() const -> player const& { return players[!turn]; }
    [[nodiscard]] auto loser() const -> player const& { return players[turn]; }
    [[nodiscard]] auto done() const -> bool
    {
        return std::ranges::any_of(players, [](auto const& p) { return p.score >= S; });
    }
};

auto day21(int argc, char** argv) -> int
{
    if (argc < 2) {
        throw std::runtime_error("argument required");
    }

    scn::owning_file f(argv[1], "r"); // NOLINT
    u64 p1 { 0 }; // player 1 position
    u64 p2 { 0 }; // player 2 position

    if (!scn::scan(f, "Player 1 starting position: {}\nPlayer 2 starting position: {}", p1, p2)) {
        throw std::runtime_error("could not parse input");
    }

    constexpr u64 max_score_part1 { 1000 };
    constexpr u64 max_score_part2 { 21 };

    player player1 { p1 - 1, 0 };
    player player2 { p2 - 1, 0 };

    // part 1
    game<max_score_part1> part_one { player1, player2 };
    dice d;
    while (!part_one.done()) {
        part_one.update(d(3));
    }
    fmt::print("part 1: {}\n", part_one.loser().score * d.count);

    // part 2
    robin_hood::unordered_map<u64, std::array<u64, 2>> map;
    game<max_score_part2> part_two { player1, player2 };

    std::array ways { 1UL, 3UL, 6UL, 7UL, 6UL, 3UL, 1UL }; // NOLINT
    auto simulate = [&](decltype(part_two) const& g, auto&& simulate) { // NOLINT
        // if game is done return win counts
        if (g.done()) {
            return std::array { u64 { g.turn }, u64 { !g.turn } };
        }
        // check if this game is cached
        auto hash = g.hash();
        if (auto it = map.find(hash); it != map.end()) {
            return it->second;
        }
        // else, recurse deeper and sum up the win counts
        std::array wins { 0UL, 0UL };
        for (auto v = 3; v <= 9; ++v) {
            auto f = ways[v - 3];
            auto h = g;
            h.update(v);
            auto [a, b] = simulate(h, simulate);
            wins = std::array { wins[0]+ f * a, wins[1] + f * b };
        }
        map[hash] = wins;
        return wins;
    };
    auto [a, b] = simulate(part_two, simulate);
    fmt::print("part 2: {}\n", std::max(a,b));

    return 0;
}
