#include <ring.hpp>
#include <math.hpp>

using gf2 = quotient_ring_element<uint32_t, 2>;
using gf1000000007 = quotient_ring_element<uint32_t, 1000000007>;

int main() {
    gf1000000007 x, y;
    cin >> x >> y;
    auto z = x / y;
    cout << z << ' ' << y * z << endl;
    return 0;
}