#pragma once

#include <utility>

#include <ka/common/log.hpp>
#include <ka/common/pragma.hpp>

#if defined(__cpp_lib_unreachable) && (__cpp_lib_unreachable >= 202202L)
    #define _KA_UNREACHABLE std::unreachable()
#elif KA_COMPILER_IS_GCC || KA_COMPILER_IS_ANY_CLANG
    #define _KA_UNREACHABLE __builtin_unreachable()
#elif KA_COMPILER_IS_MSVC
    #define _KA_UNREACHABLE __assume(false)
#endif

#ifdef NDEBUG
    #define KA_ASSERT(condition) ((void)0)
    #define KA_PRE(condition) ((void)0)
    #define KA_POST(condition) ((void)0)
    #define KA_NESTED_ASSERT(assert_type, location, ...) ((void)assert_type, (void)(location))
    #define KA_UNREACHABLE _KA_UNREACHABLE
#else
    #define KA_NESTED_ASSERT(assert_type, location, ...)                                                               \
        do                                                                                                             \
        {                                                                                                              \
            KA_PRAGMA_WARNING_PUSH;                                                                                    \
            KA_PRAGMA_DISABLE_WARNING_UNREACHABLE_CODE;                                                                \
            /* Unreachable code warning suppression on MSVC works correctly only at the function level. */             \
            [&](const std::source_location _location) noexcept                                                         \
            {                                                                                                          \
                if (std::is_constant_evaluated())                                                                      \
                {                                                                                                      \
                    if (!(__VA_ARGS__))                                                                                \
                    {                                                                                                  \
                        KA_PRAGMA_WARNING_PUSH;                                                                        \
                        KA_PRAGMA_DISABLE_WARNING_THROW_IN_NOEXCEPT;                                                   \
                        throw "Compile-time assertion failed";                                                         \
                        KA_PRAGMA_WARNING_POP;                                                                         \
                    }                                                                                                  \
                }                                                                                                      \
                else                                                                                                   \
                {                                                                                                      \
                    try                                                                                                \
                    {                                                                                                  \
                        if (!(__VA_ARGS__))                                                                            \
                        {                                                                                              \
                            ::ka::__assert_detail::assert_handler(assert_type, _location, #__VA_ARGS__, false);        \
                            _KA_UNREACHABLE;                                                                           \
                        }                                                                                              \
                    }                                                                                                  \
                    catch (...)                                                                                        \
                    {                                                                                                  \
                        ::ka::__assert_detail::assert_handler(assert_type, _location, #__VA_ARGS__, true);             \
                        _KA_UNREACHABLE;                                                                               \
                    }                                                                                                  \
                }                                                                                                      \
            }(location);                                                                                               \
            KA_PRAGMA_WARNING_POP;                                                                                     \
        } while (false)
    #define KA_ASSERT(...) KA_NESTED_ASSERT("Assertion", std::source_location::current(), __VA_ARGS__)
    #define KA_PRE(...) KA_NESTED_ASSERT("Precondition", std::source_location::current(), __VA_ARGS__)
    #define KA_POST(...) KA_NESTED_ASSERT("Postcondition", std::source_location::current(), __VA_ARGS__)
    #define KA_UNREACHABLE                                                                                             \
        do                                                                                                             \
        {                                                                                                              \
            ::ka::__assert_detail::                                                                                    \
                assert_handler("Unreachable", std::source_location::current(), "unreachable", false);                  \
            _KA_UNREACHABLE;                                                                                           \
        } while (false)
#endif

namespace ka::__assert_detail
{

[[noreturn]] inline void assert_handler(
    const std::string_view assert_type,
    const std::source_location & location,
    const std::string_view condition,
    const bool exception) noexcept
{
    log_assert(assert_type, condition, exception, location);
    std::abort();
}

} // namespace ka::__assert_detail
