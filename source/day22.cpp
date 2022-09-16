#include "advent.hpp"
#include <numeric>
#include <ranges>

// close-ended interval [a, b]
struct interval {
    i64 a;
    i64 b;

    static auto empty() { return interval{std::numeric_limits<i64>::max(), std::numeric_limits<i64>::min()}; } 
    [[nodiscard]] auto isempty() const { return a > b; }
    [[nodiscard]] auto contains(i64 v) const { return a <= v && v <= b; }
    [[nodiscard]] auto intersects(interval const& iv) const {
        return !iv.isempty() && (contains(iv.a) || contains(iv.b) || iv.contains(a) || iv.contains(b));
    }
    [[nodiscard]] auto width() const { return isempty() ? 0 : b - a + 1; }

    [[nodiscard]] auto operator&(interval const& iv) const {
        if (isempty() || iv.isempty()) { return interval::empty(); }
        if (!intersects(iv)) { return interval::empty(); }
        return interval{ std::max(a, iv.a), std::min(b, iv.b) };
    }

    [[nodiscard]] auto operator|(interval const& iv) const {
        if (isempty()) { return iv; }
        if (iv.isempty()) { return *this; }
        return interval{std::min(a, iv.a), std::max(b, iv.b)};
    }
};

struct cuboid {
    interval x; // x dimension
    interval y; // y dimension
    interval z; // z dimension
    i64 s;      // sign

    [[nodiscard]] auto volume() const { return x.width() * y.width() * z.width(); }
    [[nodiscard]] auto intersects(cuboid const& c) const { return x.intersects(c.x) && y.intersects(c.y) && z.intersects(c.z); }
    [[nodiscard]] auto operator&(cuboid const& c) const { return cuboid{ x & c.x, y & c.y, z & c.z, 0 }; };
};

auto day22(int argc, char **argv) -> int {
    if (argc < 2) {
        throw std::runtime_error("argument required");
    }

    scn::owning_file f(argv[1], "r");
    std::vector<std::string> lines;
    auto result = scn::scan_list_ex(f, lines, scn::list_separator(' '));
    std::vector<cuboid> cuboids;

    for (auto i = 0UL; i < lines.size(); i += 2) {
        auto v = lines[i] == "on" ? +1 : -1;
        i64 x1, y1, z1; // NOLINT
        i64 x2, y2, z2; // NOLINT
        auto result = scn::scan(lines[i+1], "x={}..{},y={}..{},z={}..{}", x1, x2, y1, y2, z1, z2);

        interval x{x1, x2};
        interval y{y1, y2};
        interval z{z1, z2};
        cuboids.push_back({x, y, z, v});
    }

    auto count_cubes = [&](auto&& cuboids, auto&& condition) {
        std::vector<cuboid> processed;
        for (auto const& c : cuboids) {
            if (condition(c)) { continue; }

            auto sz = processed.size();
            for (decltype(sz) i = 0; i < sz; ++i) {
                auto const& p = processed[i];

                if (c.intersects(p)) {
                    auto x = p & c;
                    if (c.s == p.s) {
                        x.s = -c.s;
                    } else if (c.s == 1 && p.s == -1) {
                        x.s = +1;
                    } else {
                        x.s = c.s * p.s;
                    }
                    processed.push_back(x);
                }
            }
            if (c.s == 1) {
                processed.push_back(c);
            }
        }
        return std::transform_reduce(processed.begin(), processed.end(), 0L, std::plus{}, [](auto const& c) { return c.volume() * c.s; });
    };

    constexpr i64 limit = 50;
    auto cond1 = [](auto const& c) { return c.x.a < -limit || c.y.a < -limit || c.z.a < -limit || c.x.b > limit || c.y.b > limit || c.z.b > limit; };
    fmt::print("part 1: {}\n", count_cubes(cuboids, cond1));
    auto cond2 = [](auto const& c) { return false; };
    fmt::print("part 2: {}\n", count_cubes(cuboids, cond2));

    return 0;
}
