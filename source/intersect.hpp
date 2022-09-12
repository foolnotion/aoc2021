#ifndef INTERSECT_HPP
#define INTERSECT_HPP

#include <span>
#include <vector>

namespace detail {
    // this method only works when the hash vectors are sorted
    template<typename T, size_t S = size_t{32} / sizeof(T)>
    inline auto count_intersect(std::span<T> lhs, std::span<T> rhs) noexcept -> size_t
    {
        T const *p0 = lhs.data(), *p_s = p0 + (lhs.size() & (-S)), *p_n = p0 + lhs.size();
        T const *q0 = rhs.data(), *q_s = q0 + (rhs.size() & (-S)), *q_n = q0 + rhs.size();
        T const *p = p0, *q = q0;

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
            std::span<T const>(x, len_x),
            std::span<T const>(y, len_y)
        );
    }

} // namespace detail

#endif
