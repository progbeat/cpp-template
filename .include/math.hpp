#ifndef CPP_TEMPLATE_MATH_HPP_
#define CPP_TEMPLATE_MATH_HPP_

namespace math {

template <class X, class N>
inline enable_if_t<std::is_integral<N>::value, X> pow(X x, N n) {
    for (X r(1); ; ) {
        if (n & 1) r *= x;
        if (n /= 2) x *= x;
        else return r;
    }
}

}  // namespace math

#endif  // CPP_TEMPLATE_MATH_HPP_