#undef NDEBUG
#include <ka/common/assert.hpp>

// clang-format off

FUNC_SPEC int the_answer() NOEXCEPT_SPEC
{
    KA_ASSERT(ASSERT_COND);
    return 42;
}

// clang-format on

static_assert(the_answer() == 42);
