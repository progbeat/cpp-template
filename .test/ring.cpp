#include <ring.hpp>
#include <math.hpp>

using int_mod = ring_element<static_quotient_ring<1000000007>>;

int main() {
    int_mod x, y;
    cin >> x >> y;
    auto z = x / y;
    cout << z << ' ' << y * z << endl;
    return 0;
}