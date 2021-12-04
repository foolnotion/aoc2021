#include "advent.hpp"

#include <fstream>
#include <robin_hood.h>
#include <scn/scn.h>

enum direction { forward, up, down };

const robin_hood::unordered_flat_map<std::string, direction> dex {
    { "forward", direction::forward },
    { "up", direction::up },
    { "down", direction::down }
};

auto day02(int argc, char** argv) -> int {

    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::string line;

    using command = std::pair<direction, int>;

    int pos{0};
    int depth{0};

    std::vector<command> pairs;
    std::vector<std::string> tokens;
    while (std::getline(infile, line)) {
        util::tokenize(line, ' ', tokens);
        auto res = scn::scan_value<int>(tokens[1]);
        ENSURE(res);
        auto it = dex.find(tokens[0]);
        ENSURE(it != dex.end());
        pairs.emplace_back(it->second, res.value());
    }

    // part 2
    int aim2{0};
    int pos2{0};
    int dep2{0};

    for(auto const& [cmd, val] : pairs) {
        switch(cmd) {
            case direction::forward:
                pos += val;
                pos2 += val;
                dep2 += aim2 * val;
                break;
            case direction::up:
                depth -= val;
                aim2 -= val;
                break;
            case direction::down:
                depth += val;
                aim2 += val;
                break;
        }
    }

    fmt::print("part1: pos: {}, depth: {}, prod: {}\n", pos, depth, pos * depth);
    fmt::print("part2: pos: {}, depth: {}, aim: {}, prod: {}\n", pos2, dep2, aim2, pos2 * dep2);
    return 0;
}
