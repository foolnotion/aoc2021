#include "advent.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <numeric>
#include <vector>
#include <tuple>
#include <ranges>
#include <string>
#include <scn/scn.h>
#include <fmt/ranges.h>
#include <set>

#include <Eigen/Core>
#include <Eigen/Geometry>

#define XXH_INLINE_ALL
#include <xxhash.h>

using std::vector;
using std::pair;
using std::string;
using std::ifstream;
using std::ranges::stable_sort;

struct def {
    static constexpr i64 nd{3};  // number of dimensions
    static constexpr i64 ncommon{12}; // number of beacons in common
};

using point = std::tuple<i64, i64, i64>;

auto intersection_count(auto const& a, auto const& b) -> i64 {
    i64 c{0};
    i64 i{0};
    i64 j{0};

    while (i < a.size() && j < b.size()) {
        auto x = a[i];
        auto y = b[j];
        c += static_cast<i64>(x == y);
        i += static_cast<i64>(x <= y);
        j += static_cast<i64>(x >= y);
    }
    return c;
}


struct scanner {
    i64 id;
    std::optional<point> position; // scanner position
    Eigen::MatrixXd beacons; // beacon positions
    Eigen::Matrix<i64, -1, -1> distances; // distance matrix between beacons
    Eigen::Matrix<i64, -1, -1> sorted_distances; // distance matrix between beacons

    explicit scanner(i64 i, Eigen::MatrixXd b)
        : id(i)
        , position(std::nullopt)
        , beacons(std::move(b))
        , distances(compute_distance_matrix(beacons))
        , sorted_distances(distances)
    {
        // sort the values so we can use intersection count
        for (auto col : sorted_distances.colwise()) {
            stable_sort(col);
        }
    }

    static auto compute_distance_matrix(auto const& m) -> Eigen::Matrix<i64, -1, -1>
    {
        Eigen::Matrix<i64, -1, -1> d = decltype(d)::Zero(m.cols(), m.cols());
        for (auto i = 0L; i < m.cols()-1; ++i) {
            for (auto j = i+1; j < m.cols(); ++j) {
                d(i, j) = d(j, i) = static_cast<i64>((m.col(i) - m.col(j)).matrix().squaredNorm());
            }
        }
        return d;
    }

    auto find_common_beacons(scanner const& s)
    {
        vector<pair<i64, i64>> map;
        for (auto i = 0L; i < distances.cols(); ++i) {
            for (auto j = 0L; j < s.distances.cols(); ++j) {
                auto count = intersection_count(sorted_distances.col(i), s.sorted_distances.col(j));
                if(count == def::ncommon) {
                    vector<i64> chosen(s.distances.cols(), 0);
                    for (auto k = 0L; k < distances.cols(); ++k) {
                        for (auto l = 0L; l < s.distances.cols(); ++l) {
                            if (!chosen[l] && distances(k, i) == s.distances(l, j)) {
                                chosen[l] = 1;
                                map.emplace_back(l, k);
                            }
                        }
                    }
                    return map;
                }
            }
        }
        return map;
    }

    auto translate(scanner const& dest, vector<pair<i64, i64>> const& map) -> void
    {
        vector<i64> idx; idx.reserve(map.size());
        vector<i64> dest_idx; dest_idx.reserve(map.size());
        for(auto [i, j] : map) {
            idx.push_back(j);
            dest_idx.push_back(i);
        }
        Eigen::MatrixXd tr = Eigen::umeyama(beacons(Eigen::all, idx), dest.beacons(Eigen::all, dest_idx)).array().round();
        auto t = tr.col(tr.cols()-1).template cast<i64>();
        position = {t(0), t(1), t(2)};
        beacons = (tr * beacons.colwise().homogeneous()).block(0, 0, beacons.rows(), beacons.cols());
    }

    friend auto operator<<(std::ostream& os, scanner const& s) -> std::ostream&;
};

auto operator<<(std::ostream& os, scanner const& s) -> std::ostream&
{
    os << "scanner " << s.id << " position: ";
    if (s.position) { 
        auto [x, y, z] = *s.position;
        os << "(" << x << "," << y << "," << z << ")\n";
    }
    else { os << "(?,?,?)\n"; }
    os << "beacons:\n";
    os << s.beacons << "\n";
    return os;
}

auto read_input(int argc, char** argv) {
    // read input
    if (argc < 2) {
        throw std::runtime_error("Error: no input.");
    }

    ifstream infile(argv[1]);
    string line;

    vector<scanner> scanners;

    vector<vector<double>> values;
    i64 id{0};
    while (std::getline(infile, line)) {
        if (line[0] == '-' && line[1] == '-') { continue; }
        if (line.empty()) {
            Eigen::MatrixXd beacons(def::nd, values.size());
            for (auto i = 0L; i < std::ssize(values); ++i) {
                beacons.col(i) = Eigen::Map<Eigen::Array<double, -1, 1>>(values[i].data(), std::ssize(values[i]));
            }
            scanners.emplace_back(id++, std::move(beacons));
            values.clear();
            continue;
        }
        values.emplace_back();
        scn::scan_list(line, values.back(), ',');
    }
    scanners[0].position = {0, 0, 0};

    return scanners;
}

auto day19(int argc, char** argv) -> int
{
    auto scanners = read_input(argc, argv);
    vector<scanner> known{scanners[0]};

    while (known.size() < scanners.size()) {
        auto sz = std::ssize(known); 
        for (auto i = 0L; i < sz; ++i) {
            auto const& s = known[i];
            for (auto& p : scanners) {
                if (p.position) { continue; }
                if (auto map = p.find_common_beacons(s); std::ssize(map) == def::ncommon) {
                    p.translate(s, map);
                    known.push_back(p);
                    break;
                }
            }
        }
    }

    std::set<point> unique_beacons;
    auto manhattan = 0L;
    for (auto const& s : scanners) {
        for (auto col : s.beacons.colwise()) {
            auto t = col.template cast<i64>();
            unique_beacons.insert({t(0), t(1), t(2)});
        }
        auto [x1, y1, z1] = *s.position;
        Eigen::Array<i64, 3, 1> v{x1, y1, z1};
        for (auto const& p : scanners) {
            auto [x2, y2, z2] = *p.position;
            Eigen::Array<i64, 3, 1> u{x2, y2, z2}; 
            manhattan = std::max(manhattan, (u - v).abs().sum());
        }
    }
    fmt::print("part 1: {}\n", unique_beacons.size());
    fmt::print("part 2: {}\n", manhattan);

    return 0;
}
