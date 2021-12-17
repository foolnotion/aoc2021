#include "advent.hpp"

#include <fstream>
#include <string>
#include <scn/scn.h>

auto day17(int argc, char** argv) -> int
{
    // read input
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::string line;
    std::getline(infile, line);

    auto x1 = 0L;
    auto x2 = 0L;
    auto y1 = 0L;
    auto y2 = 0L;

    scn::scan(line, "target area: x={}..{}, y={}..{}", x1, x2, y1, y2);

    i64 part1 = 0;
    i64 part2 = 0;

    constexpr i64 lim = 200; // whatever
    for (auto vx = 0L; vx <= lim; ++vx) {
        for (auto vy = -lim; vy <= lim; ++vy) {
            i64 ymax = 0L;
            i64 x{0};
            i64 y{0};
            i64 v{vx};
            i64 u{vy};
            while (y >= y1) {
                x += v;
                y += u;
                v -= util::sgn(v);
                u -= 1;
                ymax = std::max(y, ymax);
                if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
                    part1 = std::max(ymax, part1);
                    part2 += 1;
                    break;
                }
            }
        }
    }
    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);

    return 0;
}
