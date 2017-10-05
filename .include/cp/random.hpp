#pragma once

#include <random>

namespace cp {

struct xoroshiro128plus {
    /**
     * Generates uniform random number in the range [from, to).
     */
    template <class T>
    T uniform(T from, T to) { return uniform_impl(from, to, std::is_integral<T>()); }

    /**
     * Generates uniform random number in the range [0, to).
     */
    template <class T>
    T uniform(T to) { return uniform(T(), to); }

    /**
     * Generates normal (Gaussian) random number with mean = @p mean and stddev = @p stddev.
     */
    template <class T = double>
    T normal(T mean = T(), T stddev = T(1)) { return std::normal_distribution<T>(mean, stddev)(*this); }

    using result_type = uint32_t;

    constexpr xoroshiro128plus() = default;

    template <class Seed>
    xoroshiro128plus(Seed&& s) { seed(std::forward<Seed>(s)); }

    result_type operator()() {
        s[1] ^= s[0];
        s[0] = (s[0] << 55) ^ (s[0] >> 9) ^ s[1] ^ (s[1] << 14);
        return (s[0] + (s[1] = (s[1] << 36) ^ (s[1] >> 28))) >> 32;
    }

    static constexpr result_type min() { return result_type(); }
    static constexpr result_type max() { return ~result_type(); }

    void seed(const char* value) {
        xoroshiro128plus rng0;
        do {
            rng0();
            for (*this = rng0; *value; ++value) {
                s[0] += *value;
                operator()();
            }
        } while (s[0] == 0 && s[1] == 0);
    }

private:
    alignas(16) uint64_t s[2] = {0x2b2226c33510fa6, 0xf3ce5935970031d3};

    template <class T>
    T uniform_impl(T from, T to, std::true_type) { return std::uniform_int_distribution<T>(from, to - 1)(*this); }

    template <class T>
    T uniform_impl(T from, T to, std::false_type) { return std::uniform_real_distribution<T>(from, to)(*this); }
} static random;

}  // namespace cp