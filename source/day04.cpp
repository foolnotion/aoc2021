#include "advent.hpp"

#include <fstream>
#include <iostream>
#include <Eigen/Dense>


using board = Eigen::Array<int, -1, -1>;

auto day04(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    scn::owning_file file(argv[1], "r");
    std::vector<std::string> lines;
    (void) scn::scan_list_ex(file, lines, scn::list_separator('\n'));
    std::vector<int> numbers;
    auto line = lines.begin();
    (void) scn::scan_list_ex(*line, numbers, scn::list_separator(','));

    // the remainder of the file contains the board configurations
    int rows = 0;
    std::vector<int> values;
    std::vector<board> boards;

    constexpr int nrow{5};
    constexpr int ncol{5};
    constexpr int bingo_size{25}; // 5 x 5
    while(++line != lines.end()) {
        auto ret = scn::scan_value<int>(*line);
        if (ret.has_value()) {
            values.push_back(ret.value());
        } else {
            throw std::runtime_error("could not parse value");
        }

        if (values.size() == bingo_size) {
            boards.emplace_back(Eigen::Map<Eigen::Array<int, -1, -1, Eigen::RowMajor>>(values.data(), nrow, ncol));
            values.clear();
        }
    }

    // part 1
    std::array scores { 0UL, 0UL };
    std::vector<bool> bingoed(boards.size(), false);
    for (auto n : numbers) {
        size_t idx{0};
        for (auto& b : boards) {
            for (int i = 0; i < b.cols(); ++i) {
                for (int j = 0; j < b.rows(); ++j) {
                    if (bingoed[idx]) {
                        continue;
                    }

                    if (n == b(i, j)) {
                        b(i, j) = -1;
                    }

                    if (
                            (i == j && (b.matrix().diagonal().array() < 0).all()) // bingo on the main diagonal
                            || ((i == b.rows() - j - 1 || j == b.rows() - i - 1) && (b.matrix().transpose().diagonal().array() < 0).all()) // bingo on the other diagonal
                            || (b.col(i) < 0).all() // column bingo
                            || (b.row(j) < 0).all() // row bingo
                       ) {
                        auto sum = (b < 0).select(0, b).sum();
                        scores[scores[0] != 0] = sum * n;
                        bingoed[idx] = true;
                    }
                }
            }
            ++idx;
        }
    }
    fmt::print("{}\n", scores);
    return 0;
}
