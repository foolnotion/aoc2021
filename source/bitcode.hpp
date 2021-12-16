#include "advent.hpp"

#include <numeric>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <scn/scn.h>

enum packet_type {
    sum = 0,
    mul = 1,
    min = 2,
    max = 3,
    lit = 4,
    gt = 5,
    lt = 6,
    eq = 7
};

struct bitcode {
    static constexpr u64 group_size = 5;
    static constexpr u64 header_size = 6;

    using res_t = std::pair<i64, u64>;

    static auto parse_lit(std::string_view sv) -> res_t {
        auto ss = sv;
        fmt::memory_buffer buf;

        u64 parsed_bits{0};
        i64 v{0};

        char c{'0'};

        do {
            c = sv[0];
            scn::scan(sv.substr(1, group_size-1), "{:b2}", v);
            fmt::format_to(buf, "{:04b}", v);
            parsed_bits += group_size;
            sv.remove_prefix(group_size);
        } while (c != '0');

        scn::scan(fmt::to_string(buf), "{:b2}", v);
        return { v, parsed_bits };
    }

    auto parse_op(std::string_view sv, u64 opcode) -> res_t {
        auto ss = sv;
        u64 id{0}; scn::scan(sv.substr(0, 1), "{:b2}", id);
        sv.remove_prefix(1);
        u64 parsed_bits{1};
        std::vector<i64> values;
        if (id) {
            // the next 11 bits are a number that represents the number
            // of sub-packets immediately contained by this packet 
            constexpr auto ns_bits = 11UL; // bits representing the number of subpackets
            u64 ns{0}; scn::scan(sv.substr(0, ns_bits), "{:b2}", ns); // NOLINT
            parsed_bits += ns_bits;
            sv.remove_prefix(ns_bits);
            u64 bits{0};
            for (auto i = 0UL; i < ns; ++i) {
                ENSURE(sv.size() >= bits);
                sv.remove_prefix(bits);
                auto [v, b] = parse(sv);
                bits = b;
                values.push_back(v);
                parsed_bits += bits;
            }
        } else {
            // the next 15 bits are a number that represents the total length
            // in bits of the sub-packets contained by this packet
            constexpr auto len_bits = 15UL; // bits representing the total number of subpacket bits
            u64 len{0}; scn::scan(sv.substr(0, len_bits), "{:b2}", len);
            parsed_bits += len_bits;
            sv.remove_prefix(len_bits);
            u64 bits{0};
            while (len) {
                ENSURE(sv.size() > bits);
                sv.remove_prefix(bits);
                auto [v, b] = parse(sv);
                bits = b;
                len -= bits;
                values.push_back(v);
                parsed_bits += bits;
            }
        }
        i64 value{0};
        switch(opcode) {
            case packet_type::sum: {
                value = std::reduce(values.begin(), values.end(), 0L, std::plus<>{});
                break;
            }
            case packet_type::mul: {
                value = std::reduce(values.begin(), values.end(), 1L, std::multiplies<>{});
                break;
            }
            case packet_type::min: {
                value = *std::min_element(values.begin(), values.end());
                break;
            }
            case packet_type::max: {
                value = *std::max_element(values.begin(), values.end());
                break;
            }
            case packet_type::gt: {
                ENSURE(values.size() == 2);
                value = values[0] > values[1] ? 1 : 0;
                break;
            }
            case packet_type::lt: {
                ENSURE(values.size() == 2);
                value = values[0] < values[1] ? 1 : 0;
                break;
            }
            case packet_type::eq: {
                ENSURE(values.size() == 2);
                value = values[0] == values[1] ? 1 : 0;
                break;
            }
            default: {
            }
        }
        return { value, parsed_bits };
    }

    auto parse(std::string_view sv) -> res_t {
        if (std::all_of(sv.begin(), sv.end(), [](auto c) { return c == '0'; })) {
            return { 0, sv.size() };
        }
        u64 version{0}; scn::scan(sv.substr(0, 3), "{:b2}", version);
        u64 type{0}; scn::scan(sv.substr(3, 3), "{:b2}", type);
        
        version_sum += version;

        sv.remove_prefix(header_size);
        u64 parsed_bits{header_size};
        i64 value{0};

        if (type == packet_type::lit) {
            auto [v, b] = parse_lit(sv);
            value = v;
            parsed_bits += b;
        } else {
            auto [v, b] = parse_op(sv, type);
            value = v;
            parsed_bits += b;
        }
        return { value, parsed_bits };
    }

    auto parse() -> void {
        auto [v, _] = parse(code);
        fmt::print("part 1: {}\n", version_sum);
        fmt::print("part 2: {}\n", v);
    }

    u64 version_sum{0};

    explicit bitcode(std::string const& s) {
        fmt::memory_buffer buf;
        for (char c : s) {
            std::size_t v{0};
            scn::scan(std::string{c}, "{:x}", v);
            fmt::format_to(buf, "{:04b}", v);
        }
        code = fmt::to_string(buf);
    }

    std::string code;
};
