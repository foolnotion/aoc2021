#include "advent.hpp"

#include <scn/scn.h>

auto day01(int argc, char** argv) -> int {

    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    auto *f = std::fopen(argv[1], "re");
    scn::file file{f};
    std::vector<int> values;
    auto result = scn::scan_list(file, values, '\n');
    size_t count1{0UL};
    size_t count2{0UL};
    for (size_t i = 1; i < values.size(); ++i) {
        count1 += (values[i] > values[i-1]);
    }
    fmt::print("part 1: {}\n", count1);

    for (size_t i = 3; i < values.size(); ++i) {
        count2 += (values[i] > values[i-3]);
    }
    fmt::print("part 2: {}\n", count2);

    return 0;
}
