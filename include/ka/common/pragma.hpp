#pragma once

/// @name Compiler detection
/// @{

/// Defined to 1 when the compiler is MSVC, 0 otherwise.
#define KA_COMPILER_IS_MSVC 0
/// Defined to 1 when the compiler is GCC, 0 otherwise.
#define KA_COMPILER_IS_GCC 0
/// Defined to 1 when the compiler is clang, apple-clang or clang-cl, 0 otherwise.
#define KA_COMPILER_IS_ANY_CLANG 0
/// Defined to 1 when the compiler is clang, 0 otherwise.
#define KA_COMPILER_IS_CLANG 0
/// Defined to 1 when the compiler is clang-cl, 0 otherwise.
#define KA_COMPILER_IS_CLANG_CL 0
/// Defined to 1 when the compiler is apple-clang, 0 otherwise.
#define KA_COMPILER_IS_APPLE_CLANG 0

/// @}

/// @name Pragma syntax detection
/// @{

/// Defined to 1 when preferred pragma syntax is MSVC pragma syntax, 0 otherwise.
#define KA_PRAGMA_SYNTAX_MSVC 0
/// Defined to 1 when preferred pragma syntax is GCC/clang pragma syntax, 0 otherwise.
#define KA_PRAGMA_SYNTAX_GCC 0

/// @}

#if defined(_MSC_VER) && !defined(__clang__)
    #undef KA_COMPILER_IS_MSVC
    #define KA_COMPILER_IS_MSVC 1
    #undef KA_PRAGMA_SYNTAX_MSVC
    #define KA_PRAGMA_SYNTAX_MSVC 1
#elif defined(__GNUC__) && !defined(__clang__)
    #undef KA_COMPILER_IS_GCC
    #define KA_COMPILER_IS_GCC 1
    #undef KA_PRAGMA_SYNTAX_GCC
    #define KA_PRAGMA_SYNTAX_GCC 1
#elif defined(__clang__)
    #undef KA_COMPILER_IS_ANY_CLANG
    #define KA_COMPILER_IS_ANY_CLANG 1
    #undef KA_PRAGMA_SYNTAX_GCC
    #define KA_PRAGMA_SYNTAX_GCC 1

    #if defined(__apple_build_version__)
        #undef KA_COMPILER_IS_APPLE_CLANG
        #define KA_COMPILER_IS_APPLE_CLANG 1
    #elif defined(_MSC_VER)
        #undef KA_COMPILER_IS_CLANG_CL
        #define KA_COMPILER_IS_CLANG_CL 1
    #else
        #undef KA_COMPILER_IS_CLANG
        #define KA_COMPILER_IS_CLANG 1
    #endif
#endif

/// @name Warning management macros
/// @{

// _Pragma requires a string literal, so we stringify the argument.
// We are not targeting pre C++11 compilers, so we prefer standard _Pragma over MSVC __pragma extension.
#define KA_PRAGMA(x) _Pragma(#x)

#if KA_PRAGMA_SYNTAX_MSVC
    #define KA_PRAGMA_DISABLE_WARNING(warningNumber) KA_PRAGMA(warning(disable : warningNumber))
    #define KA_PRAGMA_WARNING_PUSH KA_PRAGMA(warning(push))
    #define KA_PRAGMA_WARNING_POP KA_PRAGMA(warning(pop))
#elif KA_PRAGMA_SYNTAX_GCC
    #define KA_PRAGMA_DISABLE_WARNING(warningName) KA_PRAGMA(GCC diagnostic ignored warningName)
    #define KA_PRAGMA_WARNING_PUSH KA_PRAGMA(GCC diagnostic push)
    #define KA_PRAGMA_WARNING_POP KA_PRAGMA(GCC diagnostic pop)
#else
    #define KA_PRAGMA_DISABLE_WARNING(warningName)
    #define KA_PRAGMA_WARNING_PUSH
    #define KA_PRAGMA_WARNING_POP
#endif

#if KA_COMPILER_IS_MSVC
    #define KA_PRAGMA_DISABLE_WARNING_THROW_IN_NOEXCEPT KA_PRAGMA_DISABLE_WARNING(4297)
#elif KA_COMPILER_IS_GCC
    #define KA_PRAGMA_DISABLE_WARNING_THROW_IN_NOEXCEPT KA_PRAGMA_DISABLE_WARNING("-Wterminate")
#else
    #define KA_PRAGMA_DISABLE_WARNING_THROW_IN_NOEXCEPT
#endif

#if KA_COMPILER_IS_MSVC
    #define KA_PRAGMA_DISABLE_WARNING_CONDITION_IS_CONSTANT KA_PRAGMA_DISABLE_WARNING(4127)
#else
    #define KA_PRAGMA_DISABLE_WARNING_CONDITION_IS_CONSTANT
#endif

#if KA_COMPILER_IS_MSVC
    #define KA_PRAGMA_DISABLE_WARNING_UNREACHABLE_CODE KA_PRAGMA_DISABLE_WARNING(4702)
#elif KA_COMPILER_IS_GCC
    #define KA_PRAGMA_DISABLE_WARNING_UNREACHABLE_CODE
#elif KA_COMPILER_IS_ANY_CLANG
    #define KA_PRAGMA_DISABLE_WARNING_UNREACHABLE_CODE KA_PRAGMA_DISABLE_WARNING("-Wunreachable-code")
#else
    #define KA_PRAGMA_DISABLE_WARNING_UNREACHABLE_CODE
#endif

/// @}
