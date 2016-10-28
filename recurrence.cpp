const int mod = 1e9 + 7;

int add(int x, int y) { return (x += y) < mod ? x : x - mod; }
int sub(int x, int y) { return (x -= y) < 0 ? x + mod : x; }
int mul(int x, int y) { return (ll)x * y % mod; }

int qpow(int x, int n) {
	int y = 1;
	for (; n; n /= 2) {
		if (n & 1) y = mul(x, y);
		x = mul(x, x);
	}
	return y;
}

vector<int> mul(vector<int> &a, vector<int> &b, vector<int> &c) {
	vector<int> r(sz(a) + sz(b));
	REP (i, sz(a)) REP (j, sz(b)) {
		r[i + j] = add(r[i + j], mul(a[i], b[j]));
	}
	for (int n = sz(r); n -->= sz(c); r.pop_back()) {
		FOR (i, 1, sz(c))
			r[n - i] = sub(r[n - i], mul(r.back(), c[i]));
	}
	return r;
}

int get(int n, int a, int b) {
	vector<int> F[2] = {{1}, {0}}, S, C = {1}, B = {1};
	for (int k = 0, m = 1, v = 1; ; ++k) {
		F[0].pb(0);
		F[1].pb(0);
		for (int i = k + 1; i >= 0; --i) {
			F[0][i] = add(F[0][i], F[1][i]);
			if (i > 0) F[1][i] = F[0][i - 1];
		}
		S.pb(0);
		for (int y = 0; y <= k; ++y)
			S[k] = add(S[k], mul(F[0][y], mul(qpow(k - y, a), qpow(y, b))));
		int w = 0;
		REP (i, sz(C)) w = add(w, mul(C[i], S[k - i]));
		if (w) {
			auto T = C;
			C.resize(max(sz(C), sz(B) + m));
			int z = mul(qpow(v, mod - 2), w);
			REP (i, sz(B)) C[i + m] = sub(C[i + m], mul(B[i], z));
			B = T;
			v = w;
			m = 1;
		} else if (++m > 2 * sz(C) + 128) {
			break;
		}
	}
	B = {0, 1};
	vector<int> R = {1};
	for (; n; n /= 2) {
		if (n & 1) R = mul(R, B, C);
		B = mul(B, B, C);
	}
	int res = 0;
	REP (i, sz(R)) res = add(res, mul(R[i], S[i]));
	return res;
}
