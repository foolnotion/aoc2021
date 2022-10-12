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
    std::array arr { { ts... } };
    return XXH_INLINE_XXH3_64bits(arr.data(), arr.size() * sizeof(decltype(arr[0])));
}

auto day24(int argc, char** argv) -> int
{

    constexpr i64 np { 3 }; // number of parameters of the monad function
    constexpr i64 nv { 14 }; // number of parameter values

    std::array<i64, np* nv> data = {
        1, 11, 6,
        1, 11, 12,
        1, 15, 8,
        26, -11, 7,
        1, 15, 7,
        1, 15, 12,
        1, 14, 2,
        26, -7, 15,
        1, 12, 4,
        26, -6, 5,
        26, -10, 12,
        26, -15, 11,
        26, -9, 13,
        26, 0, 7
    };

    std::span<i64> params(data.begin(), data.end());

    constexpr i64 dmax { 13 };
    constexpr i64 m { 26 };
    robin_hood::unordered_set<u64> seen;

    constexpr i64 ndigits { 9 };

    std::array<i64, ndigits> digits;
    std::iota(digits.rbegin(), digits.rend(), 1);

    bool found{false};
    auto check = [&](auto&& check, i64 z, i64 n, i64 d) { // NOLINT
        std::array arr { z, d };
        auto h = XXH_INLINE_XXH3_64bits(arr.data(), arr.size() * sizeof(i64));
        if (auto [it, ok] = seen.insert(h); !ok) { return false; }
        auto p = params.subspan(d * np, np);
        auto a = p[0];
        auto b = p[1];
        auto c = p[2];

        auto z0 = z;
        for (auto s : digits) {
            auto x = s != z0 % m + b;
            z = z0 / a * ((m-1) * x + 1) + (s+c) * x;

            if (d == dmax && z == 0) {
                fmt::print("{}\n", n + s);
                return true;
            }
            if (d < dmax && !found) {
                found = check(check, z, (n + s) * 10, d + 1); // NOLINT
            }
        }
        return false;
    };
    // part 1
    check(check, 0, 0, 0);

    // part 2
    seen.clear();
    found = false;
    std::iota(digits.begin(), digits.end(), 1);
    check(check, 0, 0, 0);

    return 0;
}
