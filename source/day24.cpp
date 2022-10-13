#include "advent.hpp"

#include <array>
#include <deque>
#include <numeric>

#include <fmt/ranges.h>
#include <robin_hood.h>

#define XXH_INLINE_ALL
#include <xxh3.h>

template <typename... Ts>
auto hash(Ts... ts)
{
    std::array arr { ts... };
    return XXH_INLINE_XXH3_64bits(arr.data(), arr.size() * sizeof(decltype(arr[0])));
}

auto day24(int argc, char** argv) -> int
{
    constexpr i64 np { 3 }; // number of parameters of the monad function
    constexpr i64 nv { 14 }; // number of parameter values

    std::array params = {
        std::array { 1, 11, 6 },
        std::array { 1, 11, 12 },
        std::array { 1, 15, 8 },
        std::array { 26, -11, 7 },
        std::array { 1, 15, 7 },
        std::array { 1, 15, 12 },
        std::array { 1, 14, 2 },
        std::array { 26, -7, 15 },
        std::array { 1, 12, 4 },
        std::array { 26, -6, 5 },
        std::array { 26, -10, 12 },
        std::array { 26, -15, 11 },
        std::array { 26, -9, 13 },
        std::array { 26, 0, 7 }
    };

    constexpr i64 ndigits { 9 };
    constexpr i64 dmax { 13 };
    constexpr i64 m { 26 };
    robin_hood::unordered_set<u64> seen;

    std::array<i64, ndigits> digits;
    auto check = [&](auto&& check, i64 z, i64 n, i64 d) -> std::optional<i64> { // NOLINT
        if (auto [it, ok] = seen.insert(hash(z, d)); !ok) { return {}; }
        auto [a, b, c] = params[d];

        auto z0 = z;
        for (auto s : digits) {
            auto x = s != z0 % m + b;
            // the only way z can decrease towards zero is when a=26 and x=0
            if (a == m && x != 0) { continue; }
            z = z0 / a * ((m-1) * x + 1) + (s+c) * x;
            if (d == dmax && z == 0) { return { n + s }; }
            if (d < dmax) {
                auto found = check(check, z, (n + s) * 10, d + 1);
                if (found) { return found; }
            }
        }
        return { };
    };

    // part 1
    std::iota(digits.rbegin(), digits.rend(), 1);
    auto p1 = check(check, 0, 0, 0);
    if (p1) { fmt::print("{}\n", p1.value()); }

    // part 2
    seen.clear();
    std::iota(digits.begin(), digits.end(), 1);
    auto p2 = check(check, 0, 0, 0);
    if (p2) { fmt::print("{}\n", p2.value()); }

    return 0;
}
