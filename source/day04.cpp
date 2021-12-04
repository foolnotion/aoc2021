#include "advent.hpp"

using board = Eigen::Array<int, -1, -1>;

auto day04(int argc, char** argv) -> int
{
    if (argc < 2) {
        fmt::print("Please provide an input file.\n");
        return 1;
    }

    std::ifstream infile(argv[1]); // NOLINT
    std::string line;

    // the first line represents the list of input numbers
    std::getline(infile, line);
    auto numbers = to_vec<int>(line, ',');

    // the remainder of the file contains the board configurations
    int rows = 0;
    std::vector<std::vector<int>> values;
    std::vector<board> boards;

    while(std::getline(infile, line)) {
        if (line.empty()) {
            if (rows > 0) {
                boards.emplace_back(rows, values.front().size());
                auto& b = boards.back();
                for (size_t i = 0; i < values.size(); ++i) {
                    EXPECT(values[i].size() == b.rows());
                    b.row(gsl::narrow<Eigen::Index>(i)) = as_map(values[i]);
                }
                rows = 0;
                values.clear();
                //std::cout << b << "\n\n";
            }
            continue;
        }
        auto vec = to_vec<int>(line, ' ');
        values.push_back(vec);
        ++rows;
    }

    // part 1
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
                        auto score = sum * n;
                        fmt::print("bingo at board {}! n: {}, sum: {}, score: {}\n", idx, n, sum, score);
                        std::cout << b << "\n\n";
                        bingoed[idx] = true;
                    }
                }
            }
            ++idx;
        }
    }
    return 0;
}
