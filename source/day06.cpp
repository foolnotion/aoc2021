#include "advent.hpp"

#include <algorithm>
#include <numeric>
#include <fstream>
#include <ranges>
#include <scn/scn.h>
#include <vector>

auto day06(int argc, char** argv) -> int
{
    constexpr int age_new = 8; // age of a newborn fish
    constexpr int age_old = 6; // age of an old fish after previosu age becomes 0

    if (argc < 3) {
        fmt::print("Please provide an input file and the simulation duration (days)");
        return 1;
    }

    std::ifstream infile(argv[1]); // NOLINT
    std::string str;

    std::getline(infile, str);
    std::vector<int> fish;
    auto res = scn::scan_list(str, fish, ',');
    ENSURE(res);

    int time{};
    scn::scan(std::string(argv[2]), "{}", time);

    std::vector<uint64_t> ages(age_new+1, 0);
    for(auto f : fish) {
        ++ages[f];
    }

    for (auto t : std::ranges::iota_view{0, time}) {
        std::ranges::rotate(ages, ages.begin() + 1);
        ages[age_old] += ages[age_new];
    }
    fmt::print("{}\n", std::accumulate(ages.begin(), ages.end(), 0UL));

    return 0;
}
