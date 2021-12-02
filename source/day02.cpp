#include "advent.hpp"

#include "robin_hood.h"

enum Command { FORWARD, UP, DOWN };

const robin_hood::unordered_flat_map<std::string, Command> dex {
    { "forward", Command::FORWARD },
    { "up", Command::UP },
    { "down", Command::DOWN }
};

auto day02(int argc, char** argv) -> int {

    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::string line;

    using P = std::pair<Command, int>;

    int pos{0};
    int depth{0};

    std::vector<P> pairs;
    while (std::getline(infile, line)) {
        auto tokens = split(line, ' ');
        auto res = parse_number<int>(tokens[1]);
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
            case Command::FORWARD:
                pos += val;
                pos2 += val;
                dep2 += aim2 * val;
                break;
            case Command::UP:
                depth -= val;
                aim2 -= val;
                break;
            case Command::DOWN:
                depth += val;
                aim2 += val;
                break;
        }
    }

    fmt::print("part1: pos: {}, depth: {}, prod: {}\n", pos, depth, pos * depth);
    fmt::print("part2: pos: {}, depth: {}, aim: {}, prod: {}\n", pos2, dep2, aim2, pos2 * dep2);
    return 0;
}
