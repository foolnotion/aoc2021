#include "advent.hpp"

#include <fstream>
#include <gsl/gsl_util>
#include <string>

constexpr size_t n_bit = 12;
constexpr size_t mask = ~(std::numeric_limits<size_t>::max() << n_bit);

auto day03(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    std::ifstream infile(argv[1]); // NOLINT
    std::string line;

    std::array<int, n_bit> cnt{0};
    size_t lines{0};

    std::vector<size_t> numbers;
    while(std::getline(infile, line)) {
        ++lines;
        size_t pos{0};
        auto v = static_cast<size_t>(std::stol(line, &pos, 2));
        numbers.push_back(v & mask);

        // iterate the bits in v
        while(v != 0) {
            auto const x = __builtin_ctzl(v);
            ++cnt.at(x);
            v &= (v-1);
        }
    }
    size_t gamma{0UL};
    for (size_t i = 0; i < cnt.size(); ++i) {
        if (cnt.at(i) >= lines/2) {
            gamma |= size_t{1UL} << i;
        }
    }
    auto epsilon = ~gamma & mask;
    fmt::print("gamma: {}, epsilon: {}, power: {}\n", gamma, epsilon, gamma * epsilon);

    // part two
    auto oxygen = numbers;
    auto carbon = numbers;

    auto filter = [](auto& vec, auto cond, auto comp, auto i) {
        auto count = std::count_if(vec.begin(), vec.end(), [&](auto v) { return cond(v, i); });
        auto size = vec.size();
        vec.erase(std::remove_if(vec.begin(), vec.end(), [&](auto v) {
            auto res = cond(v, i);
            return comp(count, size-count) ? !res : res;
        }), vec.end());
    };

    for (size_t i = 0; i < n_bit; ++i) {
        if (oxygen.size() > 1) {
            filter(oxygen, [](auto v, auto i) { return v & (1UL << i); }, std::greater_equal<>{}, n_bit - i - 1);
        }

        if (carbon.size() > 1) {
            filter(carbon, [](auto v, auto i) { return !(v & (1UL << i)); }, std::less_equal<>{}, n_bit - i - 1);
        }
    }
    ENSURE(oxygen.size() == 1);
    ENSURE(carbon.size() == 1);
    fmt::print("oxygen: {}, carbon dioxide: {}, life support rating: {}\n", oxygen.front(), carbon.front(), oxygen.front() * carbon.front());

    return 0;
}
