#pragma once

namespace ring_algo {

template <uint32_t Mod>
inline constexpr uint32_t mod_mul(uint32_t x, uint32_t y) {
    return (uint64_t)x * y % Mod;
}

template <uint64_t Mod>
inline constexpr uint64_t mod_mul(uint64_t x, uint64_t y) {
    uint64_t r = 0;
    for (; y; y /= 2) {
        if (y & 1) r = (r + x) - (r < Mod - x ? 0 : Mod);
        x = (x * 2) - (x < Mod / 2 ? 0 : Mod); 
    }
    return r;
}

}  // namespace ring_algo

template <class X, X Mod>
struct quotient_ring_element {
    static_assert(std::is_unsigned<X>::value, "X should be unisgned integer type");

    X value;

    constexpr quotient_ring_element() = default;

    constexpr quotient_ring_element(X value) : value(value) {}

    constexpr quotient_ring_element& operator += (const quotient_ring_element& other) {
        return *this = *this + other;
    }
    
    constexpr quotient_ring_element& operator -= (const quotient_ring_element& other) {
        return *this = *this - other;
    }

    constexpr quotient_ring_element& operator *= (const quotient_ring_element& other) {
        return *this = *this * other;
    }    

    constexpr quotient_ring_element& operator /= (const quotient_ring_element& other) {
        return *this = *this / other;
    }

    constexpr quotient_ring_element operator - (quotient_ring_element other) const {
        return {value < other.value ? value - other.value + Mod : value - other.value};
    }

    constexpr quotient_ring_element operator + (quotient_ring_element other) const {
        X x = Mod - other.value;
        return {value < x ? value + other.value : value - x};
    }

    constexpr quotient_ring_element operator / (quotient_ring_element other) const {
        X x = Mod, y = other.value;
        quotient_ring_element a = 0, b = 1;
        for (; y != 0; ) {
            std::swap(a -= quotient_ring_element(x / y) * b, b);
            std::swap(x %= y, y);
        }
        if (value % x) throw std::exception();
        return a * quotient_ring_element(value / x);
    }

    constexpr bool operator == (quotient_ring_element other) const {
        return value == other.value;
    }

    constexpr bool operator != (quotient_ring_element other) const {
        return value != other.value;
    }

    constexpr explicit operator X() const { return value; }

    constexpr explicit operator bool() const { return value != X(); }

    constexpr quotient_ring_element operator * (quotient_ring_element other) const {
        return ring_algo::mod_mul<Mod>(value, other.value);
    }

    friend std::istream& operator >> (std::istream& in, quotient_ring_element& x) {
        return in >> x.value;
    }

    friend std::ostream& operator << (std::ostream& out, const quotient_ring_element& x) {
        return out << x.value;
    }

    void check() const {
        assert(0 <= value && value < Mod);
    }
};