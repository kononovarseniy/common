# kononovarseniy/common

C++20/23 library of reusable types and functions frequently used in my projects.

# Building the library

Uses **Conan 2** + **CMake**. After first clone or adding dependencies, generate the CMake presets:

```sh
conan install . --build=missing -s build_type=Release
conan install . --build=missing -s build_type=Debug
```

This creates `CMakeUserPresets.json` with `conan-release` and `conan-debug` presets.

Then configure, build and run tests with the desired preset (replace `<preset>` with `conan-release` or `conan-debug`):

```sh
cmake --preset <preset>
cmake --build --preset <preset>
ctest --test-dir build/<preset> --output-on-failure
```

Or as a one-liner:

```sh
cmake --build --preset <preset> && ctest --test-dir build/<preset> --output-on-failure
```

# Library features

## `fixed.hpp`

Fixed-width integer and floating-point type aliases. Use this header instead of including `<cstdint>` or relying on platform-specific types.

**Type aliases:**

- `u8`, `u16`, `u32`, `u64` — unsigned integers.
- `s8`, `s16`, `s32`, `s64` — signed integers.
- `f32` (`float`), `f64` (`double`) — IEEE 754 binary floating-point types.

**Concepts:**

- `ieee_float<T>` — constrains to IEEE 754 binary floating-point types (`f32`, `f64`).

## `assert.hpp`

Macros that verify a condition evaluates to `true` and doesn't throw exceptions.
Work in both `consteval` and runtime contexts, reporting failure via source location and logging.

- `KA_ASSERT(condition)` — general assertion.
- `KA_PRE(condition)` — precondition check.
- `KA_POST(condition)` — postcondition check.
- `KA_UNREACHABLE` — marks code as unreachable. In debug builds, acts similar to `KA_ASSERT(false)`. In release builds, only hints the compiler without producing any output.

Asserts (except `KA_UNREACHABLE`) are disabled in `NDEBUG` builds.
Conditions with commas do not require extra parentheses.
In `consteval` context, a failed assertion produces a compile-time error via `throw`.
On condition failure or exception, the condition and its source location are printed and `std::abort()` is called.

## `cast.hpp`

Safe arithmetic type conversion functions. Use these instead of `static_cast` when you need to guarantee value preservation and get clear diagnostics on conversion errors.

**Concepts:**

- `SafelyConvertibleTo<Source, Target>` — true when the conversion is always lossless (proven at compile time via list-initialization rules).
- `IntegralSafelyConvertibleToIeeeFloatingPoint<Source, Target>` — true when an integral type can always be converted to a floating-point type without precision loss.

**Functions:**

- `safe_cast<Target>(value)` — performs `static_cast` only when `SafelyConvertibleTo` is satisfied (compile-time guarantee, no runtime cost).
- `exact_cast<Target>(value)` — converts preserving numeric value with runtime assertion that the round-trip `target -> source -> target` is lossless. Use when the source type allows precision loss but you guarantee the specific value fits.
- `exactly_castable_to<Target>(value)` — returns `true` if `exact_cast` would succeed for the given value. Works for all arithmetic type pairs: integer-to-integer, integer-to-float, float-to-integer, float-to-float.

## `hash.hpp`

Fowler-Noll-Vo 1a (FNV-1a) 64-bit hashing. A drop-in replacement for `std::hash` that is fast enough to use as the default hasher everywhere and more convenient thanks to extensibility via concepts.

**Classes:**

- `Hasher` — incremental FNV-1a hasher. Feed data via `update()`, obtain the digest via `digest()`. Supports raw byte ranges and any `Hashable` type.
- `Hash` — callable object that hashes a value in a single call: `Hash{}(value)`.
- `StrHash` — transparent hasher for string-like types (`std::string`, `std::string_view`, `const char*`, etc.). Suitable as the hash function for `std::unordered_set` / `std::unordered_map` with heterogeneous lookup.
- `StrEq` — transparent equality comparator for string-like types. Pairs with `StrHash` for heterogeneous string containers.

**Concepts:**

- `Hashable<T>` — true when `HashImpl<T>::update` is defined for the type.
- `HashableByMethod<T>` — true when the type has a `.hash(Hasher&)` method (auto-detected by `HashImpl`).

Built-in support: arithmetic types, enum types, string-like types, and any type with a `.hash(Hasher&)` method.

**Example** — making a custom type hashable:

```cpp
#include <ka/common/hash.hpp>

struct Point
{
    int x;
    int y;

    void hash(ka::Hasher & hasher) const noexcept
    {
        hasher.update(x);
        hasher.update(y);
    }

    [[nodiscard]] constexpr auto operator<=>(const Point &) const noexcept = default
};

static_assert(ka::Hashable<Point>);

// Use StrHash + StrEq for heterogeneous lookup in string containers:
std::unordered_set<std::string, ka::StrHash, ka::StrEq> hash_set;
hash_set.insert("hello");
bool found = hash_set.contains(std::string_view("hello"));

// Use Hash as a drop-in replacement for std::hash in containers:
std::unordered_set<Point, ka::Hash> point_set;
point_set.insert({ 1, 2 });

// Direct hasher usage:
ka::Hasher h;
h.update(Point { 1, 2 });
ka::u64 result = h.digest();

// Or in one call:
ka::u64 result2 = ka::Hash{}(Point { 1, 2 });
```

## `pragma.hpp`

Portable compiler detection and warning management macros. Use these to write cross-compiler warning push/pop blocks without `#ifdef` chains.

**Compiler detection macros** (evaluated to 0 or 1):

- `KA_COMPILER_IS_MSVC`, `KA_COMPILER_IS_GCC`, `KA_COMPILER_IS_ANY_CLANG`, `KA_COMPILER_IS_CLANG`, `KA_COMPILER_IS_CLANG_CL`, `KA_COMPILER_IS_APPLE_CLANG`
- `KA_PRAGMA_SYNTAX_MSVC`, `KA_PRAGMA_SYNTAX_GCC`

**Warning management macros:**

- `KA_PRAGMA_WARNING_PUSH` / `KA_PRAGMA_WARNING_POP` — save and restore the warning state.
- `KA_PRAGMA_DISABLE_WARNING(warning)` — disable a **compiler specific** warning by name or number.
- `KA_PRAGMA_DISABLE_WARNING_THROW_IN_NOEXCEPT` — suppresses warnings about throwing in `noexcept` functions.
- `KA_PRAGMA_DISABLE_WARNING_UNREACHABLE_CODE` — suppresses unreachable code warnings.
- `KA_PRAGMA_DISABLE_WARNING_CONDITION_IS_CONSTANT` — suppresses MSVC C4127 (constant conditional expression).

## CMake and Conan 2 integration

`ka_common` ships a helper module (`cmake/ka_common.cmake`).

### Using via Conan

```cmake
include(ka_common) # If you want to use cmake functions provided by the library.

find_package(ka_common CONFIG REQUIRED)

target_link_libraries(my_target PRIVATE ka::common)
```

## `ka_require_cpp_standard`

Sets the project C++ standard with a hard minimum and a default version.
Respects externally-defined `CMAKE_CXX_STANDARD` (e.g. from Conan).

```cmake
ka_require_cpp_standard(MIN <minimum> DEFAULT <default>)
```

- `MIN` (required) — minimum allowed C++ standard version. Fatal error if the current standard is below this.
- `DEFAULT` (optional) — standard to use when `CMAKE_CXX_STANDARD` is not already set.
  Defaults to `MIN` if omitted.

Sets `CMAKE_CXX_STANDARD_REQUIRED` to `ON` and disables extensions (`CMAKE_CXX_EXTENSIONS OFF`).

**Example:**

```cmake
ka_require_cpp_standard(MIN 20 DEFAULT 23)
```

## `ka_target`

Creates a named C++ target (`add_library` or `add_executable`) with an `alias` target in the `ka::` namespace, installs it, sets up `FILE_SET HEADERS`, and enables warnings.

```cmake
ka_target(<variable> <type> [NAME <name>])
```

- `<variable>` — variable name to receive the created target name.
- `<type>` — one of: `LIBRARY`, `INTERFACE_LIBRARY`, `EXECUTABLE`.
- `NAME` (optional) — component name used for target and alias names. If omitted, the current source directory name is used.

The created targets are named `ka_<component>` with alias `ka::<component>`.
Targets of type `LIBRARY` and `EXECUTABLE` automatically receive strict warning flags (`-Wall -Wextra -Wpedantic -Werror` on GCC/Clang, `/W4 /WX` on MSVC) via `target_enable_warnings`.

**Example:**

```cmake
# Creates ka_tilecut (alias ka::tilecut), sets output to <variable>
ka_target(current_target LIBRARY NAME tilecut)
```

## `BUILD_TESTING`

The module includes `CTest` and enables `BUILD_TESTING` by default.
When this option is `ON` (and `GTest` is found), `enable_testing()` is called and the `ka_gtest_target` function is available.

Conan can set `BUILD_TESTING` to `OFF` via `tools.build:skip_test`.

## `ka_gtest_target`

Creates a Google Test test executable from a list of source files, registers the tests with CTest, and enables strict warning flags.

**Signature:**

```cmake
ka_gtest_target(<test_target_name> SOURCES <source>...)
```

- `<test_target_name>` — name of the test executable target.
- `SOURCES` — list of test source files.

Requires `GTest` to be found via `find_package` (done automatically when `BUILD_TESTING` is `ON` via `include(CTest)` in the module).

**Example:**

```cmake
if(BUILD_TESTING)
    ka_gtest_target(my_lib_test
        SOURCES
            test/test_feature.cpp
            test/test_another.cpp
    )

    target_link_libraries(my_lib_test PRIVATE my_lib)
endif()
```

## `target_enable_warnings`

Enables strict compiler warning flags on a target.
Called automatically by `ka_target` for `LIBRARY` and `EXECUTABLE` targets, but can also be used directly.

**Signature:**

```cmake
target_enable_warnings(<target>)
```

Applies `-Wall -Wextra -Wpedantic -Werror` on GCC/Clang and `/W4 /WX` on MSVC.

## Full example

A typical downstream project that depends on `ka_common` via Conan:

```cmake
cmake_minimum_required(VERSION 3.23)

project(my_project)

include(ka_common)

ka_require_cpp_standard(MIN 20 DEFAULT 23)

ka_target(current_target LIBRARY NAME mylib)

target_sources(${current_target}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/my/header.hpp

    PRIVATE
        src/source.cpp
)

find_package(fmt CONFIG REQUIRED)

find_package(ka_common CONFIG REQUIRED)

target_link_libraries(${current_target}
    PRIVATE
        fmt::fmt
        ka::common
)

if(BUILD_TESTING)
    ka_gtest_target(${current_target}_test
        SOURCES
            test/test_example.cpp
    )

    target_link_libraries(${current_target}_test
        PRIVATE
            ${current_target}
            ka::common
    )
endif()
```

# AI usage

This project uses AI as an assistant tool in the following areas:

- **Test coverage.** AI is used to generate simple and boilerplate tests, freeing up developer time.
  Edge cases, complex scenarios and tests for critical code paths are written and verified manually.
- **Documentation.** AI improves readability of existing docs and generates boilerplate comments for public APIs to keep documentation complete and consistent.
- **Bug detection and code review.** AI is used to catch potential issues, suggest improvements and provide an additional review perspective.

All critical and production-relevant changes are reviewed and verified by a human developer before being merged.
AI-generated content is treated as a first draft that requires human judgment for correctness and appropriateness.

# License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for the full text.
