#include <gtest/gtest.h>

#undef NDEBUG
#include <ka/common/assert.hpp>
#include <ka/common/pragma.hpp>

namespace ka
{

inline namespace
{

void true_condition()
{
    const auto answer = 42;
    KA_PRAGMA_WARNING_PUSH;
    KA_PRAGMA_DISABLE_WARNING_CONDITION_IS_CONSTANT;
    KA_ASSERT(answer == 42);
    KA_PRAGMA_WARNING_POP;
}

void distopian_math()
{
    KA_PRAGMA_WARNING_PUSH;
    KA_PRAGMA_DISABLE_WARNING_CONDITION_IS_CONSTANT;
    KA_ASSERT(2 + 2 == 5);
    KA_PRAGMA_WARNING_POP;
}

void noexcept_distopian_math() noexcept
{
    KA_PRAGMA_WARNING_PUSH;
    KA_PRAGMA_DISABLE_WARNING_CONDITION_IS_CONSTANT;
    KA_ASSERT(2 + 2 == 5);
    KA_PRAGMA_WARNING_POP;
}

void throwing_condition()
{
    const auto do_throw = []() -> bool
    {
        throw std::runtime_error("I'm a runtime error");
    };
    KA_ASSERT(do_throw());
}

consteval int function_with_consteval_true_condition()
{
    KA_ASSERT(true);
    return 42;
}

consteval int function_with_consteval_noexcept_true_condition() noexcept
{
    KA_ASSERT(true);
    return 42;
}

constexpr auto consteval_function_result = function_with_consteval_true_condition();
constexpr auto consteval_noexcept_function_result = function_with_consteval_noexcept_true_condition();

template <typename...>
[[nodiscard]] int the_answer()
{
    return 42;
}

void multiargument_true_condition()
{
    KA_ASSERT(the_answer<int, double>() == 42);
}

void multiargument_false_condition()
{
    KA_ASSERT(the_answer<int, double>() != 42);
}

void reach_unreachable()
{
    KA_UNREACHABLE;
}

} // namespace

TEST(AssertSuite, condition_checked)
{
    true_condition();
    EXPECT_DEATH(distopian_math(), "Assertion failed: \\(2 \\+ 2 == 5\\)");
    EXPECT_DEATH(noexcept_distopian_math(), "Assertion failed: \\(2 \\+ 2 == 5\\)");
}

TEST(AssertSuite, exceptions_caught)
{
    EXPECT_DEATH(throwing_condition(), "Assertion failed with exception");
}

TEST(AssertSuite, consteval_asserts)
{
    EXPECT_EQ(consteval_function_result, 42);
    EXPECT_EQ(consteval_noexcept_function_result, 42);
}

TEST(AssertSuite, multiargument_condition)
{
    multiargument_true_condition();
    EXPECT_DEATH(multiargument_false_condition(), "Assertion failed: \\(the_answer<int, double>\\(\\) != 42\\)");
}

TEST(AssertSuite, unreachable)
{
    EXPECT_DEATH(reach_unreachable(), "unreachable");
}

} // namespace ka
