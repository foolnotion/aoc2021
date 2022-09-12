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

    constexpr int nb {9}; // 3x3 grid
    auto decode = [](image const& im, point p, bool g = false /*background*/) {
        auto [x, y] = p;
        int k = nb;
        std::bitset<nb> b;
        for (auto i = x - 1; i <= x + 1; ++i) {
            for (auto j = y - 1; j <= y + 1; ++j) {
                auto it = im.find({i, j});
                b[--k] = it == im.end() ? g : it->second;
            }
        }
        return b.to_ulong();
    };

    auto getlimits = [](image const& im) -> std::tuple<int, int, int, int> {
        auto [xmin, xmax] = std::ranges::minmax_element(im, std::less{}, [](auto const& t) { return t.first[0]; });
        auto [ymin, ymax] = std::ranges::minmax_element(im, std::less{}, [](auto const& t) { return t.first[1]; });
        return {xmin->first[0], xmax->first[0], ymin->first[1], ymax->first[1]};
    };

    auto enhance = [&](image im, int steps) {
        image out;
        bool lit { false };
        auto [xmin, xmax, ymin, ymax] = getlimits(im);
        for (auto s = 1; s <= steps; ++s) {
            for (auto x = xmin - s; x <= xmax + s; ++x) {
                for (auto y = ymin - s; y <= ymax + s; ++y) {
                    auto v = decode(im, { x, y }, lit);
                    out[{ x, y }] = alg[v];
                }
            }
            if (alg[0]) { lit = !lit; }
            im.swap(out);
            out.clear();
        }
        auto count = std::ranges::count_if(im, [](auto const& t) { return t.second; });
        return count;
    };
    fmt::print("part 1: {}\n", enhance(input, 2));
    fmt::print("part 2: {}\n", enhance(input, 50));

    return 0;
}
