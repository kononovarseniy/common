#include <gtest/gtest.h>

#include <array>
#include <string>
#include <string_view>
#include <unordered_set>

#include <ka/common/fixed.hpp>
#include <ka/common/hash.hpp>

namespace ka
{

constexpr u64 test_string_hash = 18007334074686647077u;

TEST(HashTestSuite, HasherIsConstructible)
{
    std::ignore = Hasher {};
}

TEST(HashTestSuite, HashIsConstructible)
{
    std::ignore = Hash {};
}

TEST(HashTestSuite, HashNothing)
{
    Hasher hasher;
    EXPECT_EQ(hasher.digest(), 0xcbf29ce484222325u);
}

TEST(HashTestSuite, HashBytes)
{
    const std::array<u8, 4> array { 0x74, 0x65, 0x73, 0x74 };
    Hasher hasher;
    hasher.update(array.data(), array.size());
    EXPECT_EQ(hasher.digest(), test_string_hash);
}

TEST(HashTestSuite, CStringIsHahable)
{
    static_assert(Hashable<const char *>);
    Hasher hasher;
    const char * str = "test";
    hasher.update(str);
    EXPECT_EQ(hasher.digest(), test_string_hash);
    EXPECT_EQ(Hash {}(str), test_string_hash);
}

TEST(HashTestSuite, CStringLiteralIsHahable)
{
    Hasher hasher;
    hasher.update("test");
    EXPECT_EQ(hasher.digest(), test_string_hash);
    EXPECT_EQ(Hash {}("test"), test_string_hash);
}

TEST(HashTestSuite, StringViewIsHahable)
{
    static_assert(Hashable<std::string_view>);
    Hasher hasher;
    hasher.update(std::string_view { "test" });
    EXPECT_EQ(hasher.digest(), test_string_hash);
    EXPECT_EQ(Hash {}(std::string_view { "test" }), test_string_hash);
}

TEST(HashTestSuite, StringIsHahable)
{
    static_assert(Hashable<std::string>);
    Hasher hasher;
    hasher.update(std::string { "test" });
    EXPECT_EQ(hasher.digest(), test_string_hash);
    EXPECT_EQ(Hash {}(std::string { "test" }), test_string_hash);
}

TEST(HashTestSuite, ClassWithoutHashMethodIsNotHashable)
{
    struct Foo final
    {
        std::string a;
    };

    static_assert(!Hashable<Foo>);
}

TEST(HashTestSuite, ClassWithHashMethodIsHashable)
{
    struct Foo final
    {
        std::string a;

        void hash(Hasher & hasher) const noexcept
        {
            hasher.update(a);
        }
    };

    static_assert(Hashable<Foo>);
    Hasher hasher;
    hasher.update(Foo { "test" });
    EXPECT_EQ(hasher.digest(), test_string_hash);
    EXPECT_EQ(Hash {}(Foo { "test" }), test_string_hash);
}

TEST(HashTestSuite, ArithmeticTypesAreHashable)
{
    static_assert(Hashable<std::size_t>);
    static_assert(Hashable<std::ptrdiff_t>);
    static_assert(Hashable<u8>);
    static_assert(Hashable<s8>);
    static_assert(Hashable<u16>);
    static_assert(Hashable<s16>);
    static_assert(Hashable<u32>);
    static_assert(Hashable<s32>);
    static_assert(Hashable<u64>);
    static_assert(Hashable<s64>);
    static_assert(Hashable<f32>);
    static_assert(Hashable<f64>);
    Hasher hasher;
    const u32 value = 0x74736574; // "test"
    hasher.update(value);
    EXPECT_EQ(hasher.digest(), test_string_hash);
    EXPECT_EQ(Hash {}(value), test_string_hash);
}

TEST(HashTestSuite, StrHashWorks)
{
    StrHash hash;
    const char * str_a = "test\0A";
    const char * str_b = "test\0B";
    ASSERT_NE(str_a, str_b);

    EXPECT_EQ(hash(std::string_view { str_a }), hash("test\0B"));
    EXPECT_EQ(hash("test\0A"), hash(std::string_view { str_b }));

    EXPECT_EQ(hash(std::string_view { str_a }), hash(str_b));
    EXPECT_EQ(hash(str_a), hash(std::string_view { str_b }));

    EXPECT_EQ(hash(std::string { str_a }), hash(str_b));
    EXPECT_EQ(hash(str_a), hash(std::string { str_b }));

    EXPECT_EQ(hash(std::string_view { str_a }), hash(std::string { str_b }));
    EXPECT_EQ(hash(std::string { str_a }), hash(std::string_view { str_b }));
}

TEST(HashTestSuite, StrEqWorks)
{
    StrEq eq;
    const char * str_a = "test\0A";
    const char * str_b = "test\0B";
    ASSERT_NE(str_a, str_b);

    EXPECT_TRUE(eq("test\0A", "test\0B"));
    EXPECT_TRUE(eq(str_a, str_b));
    EXPECT_TRUE(eq(std::string_view { str_a }, std::string_view { str_b }));
    EXPECT_TRUE(eq(std::string { str_a }, std::string { str_b }));

    EXPECT_TRUE(eq(std::string_view { str_a }, str_b));
    EXPECT_TRUE(eq(str_a, std::string_view { str_b }));

    EXPECT_TRUE(eq(std::string { str_a }, str_b));
    EXPECT_TRUE(eq(str_a, std::string { str_b }));

    EXPECT_TRUE(eq(std::string_view { str_a }, std::string { str_b }));
    EXPECT_TRUE(eq(std::string { str_a }, std::string_view { str_b }));
}

TEST(HashTestSuite, StringHashSetFunctional)
{
    {
        std::unordered_set<std::string, StrHash, StrEq> hash_set;
        EXPECT_TRUE(hash_set.emplace("A").second);
        EXPECT_TRUE(hash_set.emplace("B").second);
        EXPECT_TRUE(hash_set.emplace("C").second);

        EXPECT_TRUE(hash_set.contains("A"));
        EXPECT_TRUE(hash_set.contains(std::string { "B" }));
        EXPECT_TRUE(hash_set.contains(std::string_view { "C" }));
    }
    {
        std::unordered_set<std::string_view, StrHash, StrEq> hash_set;
        EXPECT_TRUE(hash_set.emplace("A").second);
        EXPECT_TRUE(hash_set.emplace("B").second);
        EXPECT_TRUE(hash_set.emplace("C").second);

        EXPECT_TRUE(hash_set.contains("A"));
        EXPECT_TRUE(hash_set.contains(std::string { "B" }));
        EXPECT_TRUE(hash_set.contains(std::string_view { "C" }));
    }
    {
        std::unordered_set<const char *, StrHash, StrEq> hash_set;
        EXPECT_TRUE(hash_set.emplace("A\0emplaced").second);
        EXPECT_TRUE(hash_set.emplace("B\0emplaced").second);
        EXPECT_TRUE(hash_set.emplace("C\0emplaced").second);

        EXPECT_TRUE(hash_set.contains("A\0tested"));
        EXPECT_TRUE(hash_set.contains(std::string { "B\0tested" }));
        EXPECT_TRUE(hash_set.contains(std::string_view { "C\0tested" }));
    }
}

} // namespace ka
