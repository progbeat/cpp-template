#include <bits/stdc++.h>
#define UN(v) sort(all(v)), (v).erase(unique(all(v)), (v).end())
#define FOR(i, a, b) for (int i(a), _B_##i(b); i < _B_##i; ++i)
#define CL(a, b) memset(a, b, sizeof a)
#define all(a) (a).begin(), (a).end()
#define REP(i, n) FOR(i, 0, n)
#define sz(a) int((a).size())
#define ll int64_t
#define pb push_back
#define Y second
#define X first
#ifndef LOCAL
#define NDEBUG
#endif

using namespace std;

typedef pair<int, int> pii;

const int mod = 1e9 + 7;

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

    vector<X> pmul(vector<X> &a, vector<X> &b) {
        vector<X> r(sz(a) + sz(b));
        REP (i, sz(a)) REP (j, sz(b)) {
            r[i + j] = add(r[i + j], mul(a[i], b[j]));
        }
        for (int n = sz(r); n -->= sz(f); r.pop_back()) {
            FOR (i, 1, sz(f))
                r[n - i] = sub(r[n - i], mul(r.back(), f[i]));
        }
        return r;
    }

    X solve(ll n) {
        vector<X> x = {0, 1}, r = {1};
        for (; n; n /= 2) {
            if (n & 1) r = pmul(r, x);
            x = pmul(x, x);
        }
        X res = 0;
        REP (i, sz(r)) res = add(res, mul(r[i], s[i]));
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
            g = t;
            q = p;
            m = 1;
        } else if (++m > 128) {
            return true;
        }
        return false;
    }
};

int main() {
    LinearRecurrence<int> a;
    auto f = [](int x) {
        return qpow(x, x % 10);
    };
    for (int x = 0; ; ++x) {
        if (a.next(f(x))) {
            cout << x << endl;
            break;
        }
    }
    for (int x : a.f)
        cout << x << ' ';
    cout << endl;
    REP (i, 20) {
        int x = 1000000 + i * 123;
        cout << i << ' ' << f(x) << ' ' << a.solve(x) << endl;
    }
    return 0;
}
