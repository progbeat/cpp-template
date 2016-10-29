#include <bits/stdc++.h>
#define UN(v) sort(all(v)), (v).erase(unique(all(v)), (v).end())
#define FOR(i, a, b) for (decltype(a) i(a), _B_##i(b); i < _B_##i; ++i)
#define CL(a, b) memset(a, b, sizeof a)
#define all(a) (a).begin(), (a).end()
#define REP(i, n) FOR(i, 0, n)
#define sz(a) int((a).size())
#define ll int64_t
#define pb push_back
#define Y second
#define X first

using namespace std;

typedef pair<int, int> pii;

int mod;

int add(int x, int y) { return (x += y) < mod ? x : x - mod; }
int sub(int x, int y) { return (x -= y) < 0 ? x + mod : x; }
int mul(int x, int y) { return (ll)x * y % mod; }

template <class X>
X qpow(X x, X n) {
	X y{1};
	for (; n; n /= 2) {
		if (n & 1) y = mul(x, y);
		x = mul(x, x);
	}
	return y;
}

int inv(int x) { return qpow(x, mod - 2); }

template <class X>
struct LinearRecurrence {
    vector<X> s, f = {1}, g = {1};
    int m = 1;
    X q = 1;
    
    void preduce(vector<X> &a) const {
        for (int n = sz(a); n -->= sz(f); a.pop_back())
            FOR (i, 1, sz(f))
                a[n - i] = sub(a[n - i], mul(a.back(), f[i]));
    }
    
    void pmulx(vector<X> &a) const {
        a.emplace(a.begin());
        preduce(a);
    }
    
    vector<X> pmul(vector<X> &a, vector<X> &b) const {
        vector<X> r(sz(a) + sz(b) - 1);
        REP (i, sz(a)) REP (j, sz(b))
            r[i + j] = add(r[i + j], mul(a[i], b[j]));
        preduce(r);
        return r;
    }

    X solve(ll n) const { return solve(n, n + 1)[0]; }
    
    vector<X> solve(ll from, ll to) const {
        vector<X> x = {0, 1}, r = {1};
        for (int i = 62; i >= 0; --i) {
            r = pmul(r, r);
            if ((from >> i) & 1) pmulx(r);
        }
        vector<X> res;
        FOR (n, from, to) {
            res.emplace_back();
            REP (i, sz(r)) res.back() = add(res.back(), mul(r[i], s[i]));
            pmulx(r);
        }
        return res;
    }
    
    bool next(X x) {
        int k = sz(s);
        s.pb(x);
        X p = 0;
        REP (i, sz(f)) p = add(p, mul(f[i], s[k - i]));
        if (p) {
            auto t = f;
            f.resize(max(sz(f), sz(g) + m));
            X z = mul(p, inv(q));
            REP (i, sz(g)) f[i + m] = sub(f[i + m], mul(g[i], z));
            for (; f.back() == 0; f.pop_back());
            g = t;
            q = p;
            m = 1;
        } else if (++m > 128) {
            return false;
        }
        return true;
    }
};

void test1() {
    mod = 1e9 + 7;
    LinearRecurrence<int> a;
    auto f = [](ll x) {
        return qpow(x % mod, x % 10);
    };
    for (int x = 0; a.next(f(x)); ++x);
    assert(sz(a.f) == 101);
    REP (i, 100) {
        ll x = 1000000ll + i * 65537;
        assert(f(x) == a.solve(x));
    }
}

void test2() {
    mod = 2;
    mt19937 generator;
    LinearRecurrence<int> r;
    ll n = 0;
    for (; ; ) {
        uint32_t x = generator();
        ++n;
        if (!r.next(x & 1)) break;
    }
    cout << sz(r.f) << endl;
    assert(sz(r.f) == 19938);
    const int SKIP = 10000000;
    const int N = 1000;
    REP (k, SKIP) {
        generator();
        ++n;
    }
    vector<int> c = r.solve(n, n + N);
    REP (k, N) {
        uint32_t x = 0;
        assert((generator() & 1) == c[k]);
    }
}

int main() {
    test1();
    test2();
    return 0;
}
