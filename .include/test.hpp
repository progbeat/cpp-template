#pragma once

#include <sstream>
#include <vector>

#define CONCATENATE_IMPL(x, y)  x##y
#define CONCATENATE(x, y)       CONCATENATE_IMPL(x, y)

#define TEST_CASE_IMPL(cn, tn, as)                              \
	struct cn : tests::test_case_base {                         \
		const char* test_name() const override { return tn; }   \
        const char* run_args() const override { return as; }    \
        void body() const override;                             \
        cn() { tests::all_test_cases().push_back(this); }       \
        static cn initializer;                                  \
    } cn::initializer;                                          \
    inline void cn::body() const

#define TEST_CASE(name, ...)        \
    TEST_CASE_IMPL(CONCATENATE(_tesT_casE_, __COUNTER__), #name, #__VA_ARGS__)

#define ASSERT_EQUAL(expected, actual, ...)     \
    tests::assert_equal(__FILE__, __LINE__, expected, actual, ##__VA_ARGS__)

#define ASSERT_TRUE(actual)     ASSERT_EQUAL(true, actual)
#define ASSERT_FALSE(actual)    ASSERT_EQUAL(false, actual)

namespace tests {

inline const char* relative_path(const char* path) {
    size_t pos = std::string(path).find(".test/");
    if (pos != std::string::npos) path += pos;
    return path;
}

template <class T1, class T2>
inline void assert_equal(const char* filename, int line, const T1& expected, const T2& actual) {
    if (expected == actual) return;
    std::stringstream ss;
    ss << "expected = " << expected << ", actual = " << actual;
    ss << "; " << relative_path(filename) << ":" << line;
    throw ss.str();
}

inline void assert_equal(const char* filename, int line, double expected, double actual, double eps) {
    double num = std::abs(expected - actual);
    double denom = std::max(std::max(std::abs(expected), std::abs(actual)), 1.0);
    if (num <= eps * denom) return;
    std::stringstream ss;
    ss << "expected = " << expected << ", actual = " << actual << ", error = " << num / denom;
    ss << "; " << relative_path(filename) << ":" << line;
    throw ss.str();
}

struct test_case_base {
    virtual const char* test_name() const = 0;
    virtual const char* run_args() const = 0;
    virtual void body() const = 0;
    
    bool run() {
        try {
            std::string tn = test_name();
            if (tn.size() > 2 && tn.front() == '"' && tn.back() == '"')
                tn = tn.substr(1, tn.size() - 2);
            std::cerr << "[" << tn << "]: ";
            body();
            std::cerr << "ok" << std::endl;
            return true;
        } catch (const std::string& fail_message) {
            std::cerr << "failed: " << fail_message << std::endl;
            return false;
        }
    }
};

std::vector<test_case_base*>& all_test_cases() { static std::vector<test_case_base*> instance; return instance; }

inline int run(int argc, const char* argv[]) {
    for (auto tc_ptr : all_test_cases()) {
        if (!tc_ptr->run())
            return 1;
    }
    return 0;
}

}  // namespace tests