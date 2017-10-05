#pragma once

#include <cp/diagnostics/detail/to_string.hpp>
#include <cp/diagnostics/detail/split_va_args.hpp>
#include <cp/string/split.hpp>
#include <cp/string/trim.hpp>
#include <cp/lexical_cast.hpp>
#include <cp/utility.hpp>
#include <cp/random.hpp>
#include <cp/time.hpp>

#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>

#define CONCATENATE_IMPL(x, y)  x##y
#define CONCATENATE(x, y)       CONCATENATE_IMPL(x, y)

#define TEST_IMPL(cn, as)                                           \
	struct cn : cp::tests::detail::test_base {                      \
        const char* filename() const override { return __FILE__; }  \
        int line() const override { return __LINE__; }              \
        const char* args() const override { return as; }            \
        void body(cp::xoroshiro128plus&) const override;            \
        cn() { cp::tests::detail::all_tests().push_back(this); }    \
        static cn initializer;                                      \
    } cn::initializer;                                              \
    inline void cn::body(cp::xoroshiro128plus& random) const

#define TEST(...)                   TEST_IMPL(CONCATENATE(_tesT_, __COUNTER__), #__VA_ARGS__)

#define ASSERT_EQUAL                cp::tests::detail::assert_impl(__FILE__, __LINE__).check<true>
#define ASSERT_NOT_EQUAL            cp::tests::detail::assert_impl(__FILE__, __LINE__).check<false>
#define ASSERT_TRUE                 cp::tests::detail::assert_impl(__FILE__, __LINE__).check_true
#define ASSERT_FALSE                cp::tests::detail::assert_impl(__FILE__, __LINE__).check_false
#define ASSERT_THROWS(expression)   cp::tests::detail::assert_throws(__FILE__, __LINE__, [&]{ (void)(expression); })

namespace cp { namespace tests {

namespace detail {

struct assert_impl {
    const char* filename;
    int line;
    assert_impl(const char* filename, int line) : filename(filename), line(line) {}

    template <bool Equal, class T1, class T2>
    inline void check(const T1& expected, const T2& actual) const {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
        if ((expected == actual) == Equal) return;
#pragma clang diagnostic pop
        std::stringstream ss;
        if (!Equal) ss << "not ";
        ss << "expected = " << diagnostics::detail::to_string(expected)
           << ", actual = " << diagnostics::detail::to_string(actual);
        ss << "; line = " << line;
        throw ss.str();
    }

    template <bool Equal>
    inline void check(double expected, double actual, double eps) const {
        double num = std::abs(expected - actual);
        double denom = std::max(std::max(std::abs(expected), std::abs(actual)), 1.0);
        if ((num <= eps * denom) == Equal) return;
        std::stringstream ss;
        if (!Equal) ss << "not ";
        ss << "expected = " << expected << ", actual = " << actual << ", error = " << num / denom;
        ss << "; line = " << line;
        throw ss.str();
    }

    inline void check_true(bool condition) const { check<true>(true, condition); }

    inline void check_false(bool condition) const { check<true>(false, condition); }
};

template <class F>
inline void assert_throws(const char* filename, int line, const F& f) {
    bool ok = true;
    try { f(); ok = false; } catch(...) {}
    if (ok) return;
    std::stringstream ss;
    ss << "expected exception";
    ss << "; line = " << line;
    throw ss.str();
}

static const char* repeat_key = "repeat";
static const char* time_limit_key = "time_limit";

struct test_base {
    virtual const char* filename() const = 0;
    virtual int line() const = 0;
    virtual const char* args() const = 0;
    virtual void body(xoroshiro128plus& random) const = 0;
    
    bool run() {
        using diagnostics::detail::to_string;
        cp::stopwatch stopwatch;
        try {
            cp::xoroshiro128plus random(args());
            auto args = diagnostics::detail::split_va_args(this->args());
            for (auto& v : args) {
                v = trim(v);
            }
            auto name = args.empty() ? "" : args[0];
            if (name.size() > 2 && name.front() == '"' && name.back() == '"')
                name = name.substr(1, name.size() - 2);
            std::map<std::string, std::pair<std::string, std::string>> params;
            for (size_t i = 1; i < args.size(); ++i) {
                auto kv = split(args[i], '=');
                if (kv.size() != 2) continue;
                params[trim(kv[0])] = {trim(kv[1]), args[i]};
            }
            std::cerr << name << ": ";
            auto try_get_param = [&params, this](const char* key, auto& value) -> bool {
                auto it = params.find(key);
                if (it == params.end()) return false;
                if (!TRY_ASSIGN(value, lexical_cast<std::decay_t<decltype(value)>>(it->second.first))) {
                    throw "param " + to_string(key) + " has invalid value " + to_string(it->second.first) +
                          "; line = " + to_string(line());
                }
                params.erase(it);
                return true;
            };
            int repeat = 1;
            double time_limit = 60;
            try_get_param(repeat_key, repeat);
            try_get_param(time_limit_key, time_limit);
            if (!params.empty()) {
                std::string unknown_params;
                for (auto& kv : params) {
                    if (!unknown_params.empty()) unknown_params += ", ";
                    unknown_params += kv.second.second;
                }
                throw "unknown param" + ((params.size() > 1 ? "s: " : ": ") + unknown_params) + 
                      "; line = " + to_string(line());
            }
            stopwatch.start();
            for (int k = 0; k < repeat; ++k) {
                body(random);
            }
            double elapsed_seconds = stopwatch.seconds();
            if (elapsed_seconds > time_limit) {
                throw "time limit exceeded; line = " + to_string(line());
            }
            std::cerr << "passed [ET = " << std::fixed << std::setprecision(3) << elapsed_seconds << "s]" << std::endl;
            return true;
        } catch (const std::string& fail_message) {
            double elapsed_seconds = stopwatch.seconds();
            std::cerr << "failed: " << fail_message 
                      << " [ET = " << std::fixed << std::setprecision(3) << elapsed_seconds << "s]" << std::endl;
            return false;
        }
    }
};

std::vector<test_base*>& all_tests() { static std::vector<test_base*> instance; return instance; }

}  // namespace detail

inline int run(int argc, const char* argv[]) {
    using namespace detail;
    std::map<std::string, std::vector<test_base*>> tests_per_file;
    const int total = (int)all_tests().size();
    if (total == 0) {
        std::cerr << "There are no tests :(" << std::endl;
        return 0;
    }
    for (auto ptr : all_tests()) {
        tests_per_file[ptr->filename()].push_back(ptr);
    }
    std::string common_prefix = all_tests().front()->filename();
    int common_prefix_size = (int)common_prefix.size();
    for (auto file_and_tests : tests_per_file) {
        const auto& filename = file_and_tests.first;
        while (common_prefix_size > 0 && filename[common_prefix_size - 1] != common_prefix[common_prefix_size - 1])
            --common_prefix_size;
    }
    int num_ok = 0;
    for (auto file_and_tests : tests_per_file) {
        auto filename = file_and_tests.first.substr(common_prefix_size);
        const auto& tests = file_and_tests.second;
        if (!filename.empty()) {
            std::cerr << filename << ":" << std::endl;
        }
        for (auto tc_ptr : tests) {
            num_ok += tc_ptr->run();
        }
        std::cerr << std::endl;
    }
    if (num_ok == total) {
        std::cerr << "Passed all tests." << std::endl;
    } else {
        std::cerr << "Failed " << total - num_ok << "/" << total << " tests." << std::endl;
    }
    return 0;
}

}}  // namespace cp::tests
