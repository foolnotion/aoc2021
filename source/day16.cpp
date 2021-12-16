#include "advent.hpp"
#include "bitcode.hpp"

#include <array>
#include <vector>
#include <fstream>
#include <ranges>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <scn/scn.h>

using std::vector;
using std::string;
using std::string_view;
using std::ifstream;

using std::ranges::views::reverse;

auto day16(int argc, char** argv) -> int
{
    // read input
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]); // NOLINT
    string line;
    std::getline(infile, line);
    bitcode bc(line);
    bc.parse();
    return 0;
}
