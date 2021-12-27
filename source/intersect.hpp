#ifndef INTERSECT_HPP
#define INTERSECT_HPP

#include <gsl/span>
#include <vectorclass/vectorclass.h>
#include <vector>

namespace detail {
    template<typename T, std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 8, bool> = true>
    constexpr inline auto check(T const* lhs, T const* rhs) noexcept
    {
        auto a = Vec4uq().load(lhs);
        return a == rhs[0] | a == rhs[1] | a == rhs[2] | a == rhs[3];
    }

    template<typename T, std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 4, bool> = true>
    constexpr inline auto check(T const* lhs, T const* rhs) noexcept
    {
        auto a = Vec8ui().load(lhs);
        return a == rhs[0] | a == rhs[1] | a == rhs[2] | a == rhs[3] | a == rhs[4] | a == rhs[5] | a == rhs[6] | a == rhs[7];
    }

    template<typename T, std::enable_if_t<std::is_integral_v<T> && (sizeof(T) == 4 || sizeof(T) == 8), bool> = true>
    constexpr inline auto intersect(T const* lhs, T const* rhs) noexcept -> bool
    {
        return horizontal_add(check(lhs, rhs));
    }

    // this method only works when the hash vectors are sorted
    template<typename T, size_t S = size_t{32} / sizeof(T)>
    inline auto count_intersect(gsl::span<T> lhs, gsl::span<T> rhs) noexcept -> size_t
    {
        T const *p0 = lhs.data(), *p_s = p0 + (lhs.size() & (-S)), *p_n = p0 + lhs.size();
        T const *q0 = rhs.data(), *q_s = q0 + (rhs.size() & (-S)), *q_n = q0 + rhs.size();
        T const *p = p0, *q = q0;

        while(p < p_s && q < q_s) {
            if (intersect(p, q)) {
                break;
            } 
            auto const a = *(p + S - 1);
            auto const b = *(q + S - 1);
            if (a < b) { p += S; }
            if (a > b) { q += S; }
        }

        auto const a_n = *(p_n - 1);
        auto const b_n = *(q_n - 1);
        size_t count{0};
        while(p < p_n && q < q_n) {
            auto const a = *p;
            auto const b = *q;

            if (a > b_n || b > a_n) {
                break;
            }

            count += a == b;
            p += a <= b;
            q += a >= b;
        }

        return count;
    }

    template<typename T>
    inline auto count_intersect(T const* x, size_t len_x, T const* y, size_t len_y) noexcept
    {
        return count_intersect(
            gsl::span<T const>(x, len_x),
            gsl::span<T const>(y, len_y)
        );
    }

} // namespace detail

#endif
