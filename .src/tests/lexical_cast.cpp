#include <cp/lexical_cast.hpp>
#include <cp/test.hpp>

using namespace cp;

TEST("diagnostics::detail::to_string") {
    ASSERT_EQUAL(1, lexical_cast<int>("1"));
    ASSERT_EQUAL("42", lexical_cast<std::string>(42));
    ASSERT_EQUAL(1.25, lexical_cast<double>("1.2500"));
    ASSERT_THROWS(lexical_cast<int>("1 "));
    ASSERT_THROWS(lexical_cast<int>(""));
}

int main(int argc, const char* argv[]) { return tests::run(argc, argv); }