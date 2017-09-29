#pragma once

struct {
    uint64_t operator()() {
        s[1] ^= s[0];
        s[0] = (s[0] << 55) ^ (s[0] >> 9) ^ s[1] ^ (s[1] << 14);
        return s[0] + (s[1] = (s[1] << 36) ^ (s[1] >> 28));
    }
    static constexpr uint64_t min() { return uint64_t(); }
    static constexpr uint64_t max() { return ~uint64_t(); }
private:
    alignas(16) uint64_t s[2] = {0x2b2226c33510fa6, 0xf3ce5935970031d3};
}
static random1;