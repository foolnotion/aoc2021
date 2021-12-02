#include "advent.hpp"

int day01(int argc, char** argv) {

    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    std::vector<int> values;

    std::ifstream infile(argv[1]);
    std::string line;

    while (std::getline(infile, line)) {
        if (auto v = parse_number<int>(line); v.has_value()) {
            values.push_back(v.value());
        }
    }

    size_t count1{0UL}, count2{0UL};
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
