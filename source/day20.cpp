#include "advent.hpp"

#include <bitset>
#include <fstream>
#include <iostream>
#include <robin_hood.h>
#include <xxhash.h>

namespace std {
template <class T, size_t N>
struct hash<std::array<T, N>> {
    auto operator()(std::array<T, N> const& arr) const noexcept -> std::size_t
    {
        return XXH3_64bits(arr.data(), N * sizeof(T) / sizeof(uint8_t));
    }
};
} // namespace std

auto day20(int argc, char** argv) -> int
{
    if (argc < 2) {
        throw std::runtime_error("argument missing");
    }

    std::ifstream ff(argv[1]);

    std::string line;
    std::getline(ff, line);
    std::bitset<512> alg;
    for (int i = 0; i < line.size(); ++i) {
        alg[i] = line[i] == '#';
    }

    std::getline(ff, line); // skip an empty line

    using point = std::array<int, 2>;
    using image = robin_hood::unordered_map<point, bool>;

    // read input
    image input;
    {
        int i = 0;
        while (std::getline(ff, line)) {
            for (int j = 0; j < line.size(); ++j) {
                input[{ i, j }] = line[j] == '#';
            }
            ++i;
        }
    }

    constexpr int nb { 9 }; // 3x3 grid
    auto decode = [](image const& im, point p, bool g = false /*backgound*/) {
        auto [x, y] = p;
        int k = nb;
        std::bitset<nb> b;
        for (auto i = x - 1; i <= x + 1; ++i) {
            for (auto j = y - 1; j <= y + 1; ++j) {
                --k;
                if (auto it = im.find({ i, j }); it != im.end()) {
                    b[k] = it->second;
                } else {
                    b[k] = g;
                }
            }
        }
        return b.to_ulong();
    };

    auto getlimits = [](image const& im) -> std::tuple<int, int, int, int> {
        int x1 { 0 };
        int x2 { 0 };
        int y1 { 0 };
        int y2 { 0 };

        // get new limits
        for (auto const& [p, v] : im) {
            auto [x, y] = p;
            x1 = std::min(x1, x);
            x2 = std::max(x2, x);
            y1 = std::min(y1, y);
            y2 = std::max(y2, y);
        }

        return { x1, x2, y1, y2 };
    };

    auto enhance = [&](image im, int steps) {
        image out;
        bool lit { false };
        auto [xmin, xmax, ymin, ymax] = getlimits(im);
        for (auto s = 0; s < steps; ++s) {
            for (auto x = xmin - 1; x <= xmax + 1; ++x) {
                for (auto y = ymin - 1; y <= ymax + 1; ++y) {
                    auto v = decode(im, { x, y }, lit);
                    out[{ x, y }] = alg[v];
                }
            }
            if (alg[0]) {
                lit = !lit;
            }
            im.swap(out);
            out.clear();
            --xmin;
            ++xmax;
            --ymin;
            ++ymax;
        }
        auto count = std::ranges::count_if(im, [](auto const& t) { return t.second; });
        return count;
    };
    fmt::print("part 1: {}\n", enhance(input, 2));
    fmt::print("part 2: {}\n", enhance(input, 50));

    return 0;
}
