#include <cp/ring.hpp>
#include <cp/math.hpp>
#include <cp/random.hpp>
#include <cp/test.hpp>

using namespace cp;

TEST("static_quotient_ring:arithmetic", N=1000, TL=1s) {
    using ring = static_quotient_ring<18446744073709551557ull>;  // largest prime that fit uint64_t type
    using int_mod = ring_element<ring>;
    int_mod x = random1() % ring::mod();
    int_mod y = random1() % ring::mod();
    int_mod z = random1() % (ring::mod() - 1) + 1;
    ASSERT_EQUAL(x + y, y + x);
    ASSERT_EQUAL(x + (y + z), (x + y) + z);
    ASSERT_EQUAL(int_mod(), x + (-x));
    ASSERT_EQUAL(x - y, x + (-y));
    ASSERT_EQUAL(x * z + y * z, (x + y) * z);
    ASSERT_EQUAL(x * z - y * z, (x - y) * z);
    ASSERT_EQUAL(x / z - y / z, (x - y) / z);
    ASSERT_EQUAL(x, math::pow(x, ring::mod()));
    ASSERT_EQUAL(1, x / x);
    ASSERT_THROWS(x / int_mod());
    ASSERT_THROWS(int_modulo<1000>(347) / int_modulo<1000>(222));
    ASSERT_EQUAL(1, int_modulo<256>(13) / int_modulo<256>(13));
}

int main(int argc, const char* argv[]) { return tests::run(argc, argv); }