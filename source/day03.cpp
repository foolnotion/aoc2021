#include "advent.hpp"

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
            ++cnt[x];
            v &= (v-1);
        }
    }
    size_t gamma{0UL};
    for (size_t i = 0; i < cnt.size(); ++i) {
        if (gsl::at(cnt, static_cast<gsl::index>(i)) >= lines/2) {
            gamma |= size_t{1UL} << i;
        }
    }
    auto epsilon = ~gamma & mask;
    fmt::print("gamma: {}, epsilon: {}, power: {}\n", gamma, epsilon, gamma * epsilon);

    // part two
    auto oxygen = numbers;
    auto carbon = numbers;
    while (oxygen.size() > 1 && carbon.size() > 1) {
        for (size_t i = 0; i < n_bit; ++i) {
            auto j = n_bit - i - 1;
            auto m = 1UL << j;

            if (oxygen.size() > 1) {
                auto count = std::count_if(oxygen.begin(), oxygen.end(), [&](auto v) { return v & (1UL << j); });
                auto size = oxygen.size();
                oxygen.erase(std::remove_if(oxygen.begin(), oxygen.end(), [&](auto v) {
                            auto res = v & (1UL << j);
                            return count >= (size - count) ? !res : res;
                            }), oxygen.end());
            }

            if (carbon.size() > 1) {
                auto count = std::count_if(carbon.begin(), carbon.end(), [&](auto v) { return !(v & (1UL << j)); });
                auto size = carbon.size();
                carbon.erase(std::remove_if(carbon.begin(), carbon.end(), [&](auto v) {
                            auto res = !(v & (1UL << j));
                            return count <= (size - count) ? !res : res;
                            }), carbon.end());
            }
        }
    }
    ENSURE(oxygen.size() == 1);
    ENSURE(carbon.size() == 1);
    fmt::print("oxygen: {}, carbon dioxide: {}, life support rating: {}\n", oxygen.front(), carbon.front(), oxygen.front() * carbon.front());

    return 0;
}
