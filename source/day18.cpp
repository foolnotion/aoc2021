#include "advent.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <ranges>
#include <scn/scn.h>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

using std::ifstream;
using std::ostream;
using std::string;
using std::string_view;
using std::vector;

auto split(i64 n) -> std::pair<i64, i64>
{
    auto a = static_cast<i64>(std::floor(static_cast<double>(n) / 2.0));
    auto b = static_cast<i64>(std::ceil(static_cast<double>(n) / 2.0));
    return {a, b};
}

struct node {
    std::optional<i64> value;
    i64 parent{-1};
    i64 length{1};
    i64 depth{1};

    explicit node(i64 val) : value(val) { }
    node() : value(std::nullopt) { }
};

struct parser {
    string str;
    u64 pos{0};

    static auto parse(string const& str) {
        parser p{str, 0};
        p.parse();
        return p.nodes;
    }

    auto parse() -> void {
        node n;
        // skip split / end characters (this works assuming the input is well-formed)
        while(str[pos] == ']' || str[pos] == ',') { ++pos; }
        auto c = str[pos];
        if (c == '[') {
            nodes.push_back(n);
            ++pos;
            parse(); // left
            parse(); // right
        } else if (std::isdigit(c)) {
            u64 sz{1};
            while (std::isdigit(str[pos+sz])) { ++sz; }
            i64 v{0};
            scn::scan(str.substr(pos, sz), "{}", v);
            n.value = v;
            pos += sz;
            nodes.push_back(n);
        }
    }
    vector<node> nodes;
};

struct expr {
    vector<node> nodes;

    explicit expr(string const& str)
        : nodes(parser::parse(str))
    {
        update();
    }

    explicit expr(vector<node> vec)
        : nodes(std::move(vec))
    {
        update();
    }

    auto subspan(i64 i) -> std::span<node> {
        return std::span<node>(nodes.data() + i, nodes[i].length);
    }

    auto subspan(i64 i) const -> std::span<node const> {
        return std::span<node const>(nodes.data() + i, nodes[i].length);
    }

    auto size() const -> i64 { return std::ssize(nodes); }

    auto reduce() -> expr&;

    auto magnitude() -> i64;

    auto update() -> expr& {
        // update length
        for(auto i = size() - 1; i >= 0; --i) {
            auto& n = nodes[i];
            // reset values so we can update them
            n.length = 1;
            n.depth = 1;
            n.parent = -1;

            if (n.value) {
                continue;
            }
            for (auto k = 0L, j = i+1; k < 2; ++k, j += nodes[j].length) {
                auto& c = nodes[j];
                n.length += c.length;
                c.parent = i;
            }
        }

        // update depth
        nodes[0].depth = 1;
        for(auto i = 1L; i < size() - 1; ++i) {
            auto& n = nodes[i];
            assert(n.parent < size());
            n.depth = nodes[n.parent].depth + 1;
        }
        return *this;
    }

    auto operator+(expr const& other) -> expr {
        vector<node> sum;
        sum.reserve(nodes.size() + other.nodes.size() + 1);
        sum.emplace_back();
        std::copy(nodes.begin(), nodes.end(), std::back_inserter(sum));
        std::copy(other.nodes.begin(), other.nodes.end(), std::back_inserter(sum));
        return expr(sum);
    }

    auto operator+=(expr const& other) -> expr& {
        auto tmp = *this + other;
        nodes.swap(tmp.nodes);
        return *this;
    }

    [[nodiscard]] auto find_left(i64 i) const -> i64 {
        for (auto j = i-1; j >= 0; --j) {
            if (nodes[j].value) {
                return j;
            }
        }
        return -1;
    }

    [[nodiscard]] auto find_right(i64 i) const -> i64 {
        auto const& n = nodes[i];
        for (auto j = i+n.length; j < size(); ++j) {
            if (nodes[j].value) {
                return j;
            }
        }
        return -1;
    }
};

// helper for pretty printing
struct infix {
    explicit infix(vector<node> const& n) : nodes(n), idx(0) { }

    explicit infix(std::span<node const> n) : nodes(n), idx(0) { }

    explicit infix(expr const& t) : nodes(t.nodes), idx(0) { }

    std::span<node const> nodes;
    i64 idx;

    friend auto operator<<(ostream& os, infix&& /*infx*/) -> ostream&;

    auto print(ostream& os) -> ostream& {
        if (nodes[idx].value) {
            os << *nodes[idx].value; }
        else {
            os << "[";
            ++idx; print(os);
            os << ",";
            ++idx; print(os);
            os << "]";
        }
        return os;
    }
};

auto operator<<(ostream& os, infix&& infx) -> ostream&
{
    return infx.print(os); 
}

auto expr::reduce() -> expr& {
    bool reduced = true;
    while(reduced) {
        reduced = false;

        // look for expressions to explode
        for (auto i = 0L; i < size(); ++i) {
            auto n = nodes[i];
            if (n.length > 1 && n.depth > 4) {
                auto a = *nodes[i+1].value;
                auto b = *nodes[i+2].value;
                assert(n.length == 3);

                auto left = find_left(i);
                auto right = find_right(i);

                if (left >= 0) { *nodes[left].value += a; }
                if (right >= 0) { *nodes[right].value += b; }
                nodes[i] = node(0);
                nodes.erase(nodes.begin()+i+1, nodes.begin()+i+n.length);
                update();
                reduced = true;
                break;
            }
        }

        // look for value-expressions to split
        if (!reduced) {
            for (auto i = 0L; i < size(); ++i) {
                auto n = nodes[i];
                if (n.value && *n.value > 9) { // NOLINT
                    auto [a,b] = split(*n.value);
                    nodes[i].value = std::nullopt;
                    nodes.insert(nodes.begin()+i+1, node(a));
                    nodes.insert(nodes.begin()+i+2, node(b));
                    update();
                    reduced = true;
                    break;
                }
            }
        }
    }
    return *this;
}

auto expr::magnitude() -> i64
{
    vector<i64> magnitude(size());
    for(auto i = size()-1; i >= 0; --i) {
        auto& n = nodes[i];
        magnitude[i] = n.value
            ? *n.value
            : 3 * magnitude.at(i+1) + 2 * magnitude.at(i+1+nodes[i+1].length);
    }
    return magnitude[0];
}


auto day18(int argc, char** argv) -> int
{
    // read input
    if (argc < 2) {
        fmt::print("Error: no input.");
        return 1;
    }

    ifstream infile(argv[1]);
    string line;

    vector<expr> numbers;
    while(std::getline(infile, line)) {
        numbers.emplace_back(line);
    }
    auto sz = std::ssize(numbers);

    auto sum = numbers.front();
    for (auto i = 1L; i < sz; ++i) {
        sum += numbers[i];
        sum.reduce();
    }
    std::cout << "part 1: " << sum.magnitude() << "\n";

    i64 magmax{0};
    for (auto i = 0; i < sz-1; ++i) {
        for (auto j = i+1; j < sz; ++j) {
            auto m1 = (numbers[i] + numbers[j]).reduce().magnitude();
            auto m2 = (numbers[j] + numbers[i]).reduce().magnitude();
            magmax = std::max({ magmax, m1, m2 });
        }
    }
    std::cout << "part 2: " << magmax << "\n";

    return 0;
}
