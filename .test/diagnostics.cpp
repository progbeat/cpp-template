#include <cp/diagnostics/detail/to_string.hpp>
#include <cp/test.hpp>

using namespace cp;

TEST("diagnostics::detail::to_string") {
    using diagnostics::detail::to_string;
    ASSERT_EQUAL("true", to_string(true));
    ASSERT_EQUAL("false", to_string(false));
    ASSERT_EQUAL("\"hello\"", to_string("hello"));
    ASSERT_EQUAL("[1, 2, 3]", to_string(std::vector<int>{1, 2, 3}));
    ASSERT_EQUAL("{1, 2, 3}", to_string(std::set<int>{1, 2, 3}));
    ASSERT_EQUAL("(\"hello\", 42)", to_string(std::make_tuple("hello", 42)));
}

int main(int argc, const char* argv[]) { return tests::run(argc, argv); }