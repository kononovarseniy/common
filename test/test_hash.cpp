#include <gtest/gtest.h>

#include <array>
#include <bitset>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include <ka/common/fixed.hpp>
#include <ka/common/hash.hpp>

namespace ka
{

constexpr u64 test_string_hash = 18007334074686647077u;

struct Unhashable final
{
    std::string a;
};

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

TEST(HashTestSuite, ConstAndReferenceTypesAreHashable)
{
    struct Foo
    {
        void hash(Hasher & hasher) const
        {
            hasher.update(1);
        }
    };

    static_assert(Hashable<Foo>);
    static_assert(Hashable<std::reference_wrapper<Foo>>);
    static_assert(Hashable<const Foo>);
    static_assert(Hashable<const Foo &>);
    static_assert(Hashable<const Foo &&>);
    static_assert(Hashable<Foo &>);
    static_assert(Hashable<Foo &&>);

    constexpr Hash do_hash {};
    Foo foo;
    const auto baseline = do_hash(foo);
    EXPECT_EQ(baseline, do_hash(std::ref(foo)));
    EXPECT_EQ(baseline, do_hash(std::cref(foo)));
    EXPECT_EQ(baseline, do_hash(std::move(foo)));
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

TEST(HashTestSuite, VectorIsHashable)
{
    static_assert(Hashable<std::vector<int>>);
    static_assert(!Hashable<std::vector<Unhashable>>);

    constexpr u64 expected = 18239313798490686357u;
    EXPECT_EQ(Hash {}(std::vector<int> { 1, 2, 3 }), expected);
}

TEST(HashTestSuite, VectorBoolIsHashable)
{
    static_assert(Hashable<std::vector<bool>>);

    constexpr u64 expected = 15034027211670106815u;
    EXPECT_EQ(Hash {}(std::vector<bool> { true, false, true }), expected);
}

TEST(HashTestSuite, BitsetIsHashable)
{
    static_assert(Hashable<std::bitset<8>>);

    constexpr u64 expected = 8846950589617415811u;
    EXPECT_EQ(Hash {}(std::bitset<8> { 0b10110010u }), expected);
}

TEST(HashTestSuite, SetIsHashable)
{
    static_assert(Hashable<std::set<int>>);
    static_assert(!Hashable<std::set<Unhashable>>);

    constexpr u64 expected = 18239313798490686357u;
    EXPECT_EQ(Hash {}(std::set<int> { 1, 2, 3 }), expected);
}

TEST(HashTestSuite, UnorderedSetIsHashable)
{
    static_assert(Hashable<std::unordered_set<int>>);
    static_assert(!Hashable<std::unordered_set<Unhashable, std::hash<std::string>>>);

    constexpr u64 expected = 12371478820467099029u;
    EXPECT_EQ(Hash {}(std::unordered_set<int> { 1, 2, 3 }), expected);
}

TEST(HashTestSuite, MapIsHashable)
{
    static_assert(Hashable<std::map<std::string, int>>);
    static_assert(!Hashable<std::map<Unhashable, int>>);
    static_assert(!Hashable<std::map<int, Unhashable>>);

    constexpr u64 expected = 13597235531013623295u;
    EXPECT_EQ(Hash {}(std::map<std::string, int> { { "a", 1 }, { "b", 2 } }), expected);
}

TEST(HashTestSuite, UnorderedMapIsHashable)
{
    static_assert(Hashable<std::unordered_map<std::string, int>>);
    static_assert(!Hashable<std::unordered_map<Unhashable, int, std::hash<std::string>>>);
    static_assert(!Hashable<std::unordered_map<int, Unhashable, std::hash<std::string>>>);

    constexpr u64 expected = 3131987233830052019u;
    EXPECT_EQ(Hash {}(std::unordered_map<std::string, int> { { "a", 1 }, { "b", 2 } }), expected);
}

TEST(HashTestSuite, DequeIsHashable)
{
    static_assert(Hashable<std::deque<int>>);
    static_assert(!Hashable<std::deque<Unhashable>>);

    constexpr u64 expected = 18239313798490686357u;
    EXPECT_EQ(Hash {}(std::deque<int> { 1, 2, 3 }), expected);
}

TEST(HashTestSuite, ListIsHashable)
{
    static_assert(Hashable<std::list<int>>);
    static_assert(!Hashable<std::list<Unhashable>>);

    constexpr u64 expected = 18239313798490686357u;
    EXPECT_EQ(Hash {}(std::list<int> { 1, 2, 3 }), expected);
}

TEST(HashTestSuite, ForwardListIsHashable)
{
    static_assert(Hashable<std::forward_list<int>>);
    static_assert(!Hashable<std::forward_list<Unhashable>>);

    constexpr u64 expected = 18239313798490686357u;
    EXPECT_EQ(Hash {}(std::forward_list<int> { 1, 2, 3 }), expected);
}

TEST(HashTestSuite, PairIsHashable)
{
    static_assert(Hashable<std::pair<std::string, int>>);
    static_assert(!Hashable<std::pair<Unhashable, int>>);
    static_assert(!Hashable<std::pair<int, Unhashable>>);

    constexpr u64 expected = 1001063584236537821u;
    EXPECT_EQ(Hash {}(std::pair<std::string, int> { "a", 1 }), expected);
}

TEST(HashTestSuite, TupleIsHashable)
{
    static_assert(Hashable<std::tuple<std::string, int>>);
    static_assert(Hashable<std::tuple<>>);
    static_assert(!Hashable<std::tuple<int, Unhashable>>);

    constexpr u64 expected = 1001063584236537821u;
    EXPECT_EQ(Hash {}(std::tuple<std::string, int> { "a", 1 }), expected);
}

TEST(HashTestSuite, OptionalIsHashable)
{
    static_assert(Hashable<std::optional<int>>);
    static_assert(!Hashable<std::optional<Unhashable>>);

    constexpr u64 expected = 1790149455062925670u;
    constexpr u64 expected_empty = 12638153115695167455u;
    EXPECT_EQ(Hash {}(std::optional<int> { 42 }), expected);
    EXPECT_EQ(Hash {}(std::optional<int> {}), expected_empty);
}

TEST(HashTestSuite, UniquePtrHashesByAddress)
{
    static_assert(Hashable<std::unique_ptr<int>>);
    static_assert(Hashable<std::unique_ptr<Unhashable>>);

    const auto first = std::make_unique<int>(1);
    const auto second = std::make_unique<int>(1);

    EXPECT_EQ(Hash {}(first), Hash {}(first));
    EXPECT_NE(Hash {}(first), Hash {}(second));
    EXPECT_NE(Hash {}(std::unique_ptr<int> {}), Hash {}(first));
}

TEST(HashTestSuite, SharedPtrHashesByAddress)
{
    static_assert(Hashable<std::shared_ptr<int>>);
    static_assert(Hashable<std::shared_ptr<Unhashable>>);

    const auto first = std::make_shared<int>(1);
    const auto second = std::make_shared<int>(1);

    EXPECT_EQ(Hash {}(first), Hash {}(first));
    EXPECT_NE(Hash {}(first), Hash {}(second));
    EXPECT_NE(Hash {}(std::shared_ptr<int> {}), Hash {}(first));
}

TEST(HashTestSuite, RawPointerHashesByAddress)
{
    static_assert(Hashable<int *>);
    static_assert(Hashable<const int *>);
    static_assert(Hashable<const char *>);
    static_assert(Hashable<void *>);
    static_assert(Hashable<Unhashable *>);

    int first = 1;
    int second = 1;
    const char * str = "test";

    EXPECT_EQ(Hash {}(&first), Hash {}(&first));
    EXPECT_NE(Hash {}(&first), Hash {}(&second));
    EXPECT_NE(Hash {}(static_cast<int *>(nullptr)), Hash {}(&first));
    EXPECT_NE(Hash {}(str), test_string_hash);
}

TEST(HashTestSuite, VariantIsHashable)
{
    static_assert(Hashable<std::variant<std::string, int>>);
    static_assert(!Hashable<std::variant<int, Unhashable>>);

    constexpr u64 expected = 11500333634479716206u;
    EXPECT_EQ(Hash {}(std::variant<std::string, int> { 42 }), expected);
}

TEST(HashTestSuite, VariantWithMonostateIsHashable)
{
    static_assert(Hashable<std::variant<std::monostate, int>>);

    constexpr u64 expected = 12161962213042174405u;
    EXPECT_EQ(Hash {}(std::variant<std::monostate, int> { std::monostate {} }), expected);
}

TEST(HashTestSuite, MonostateIsHashable)
{
    static_assert(Hashable<std::monostate>);

    constexpr u64 expected = 14695981039346656037u;
    EXPECT_EQ(Hash {}(std::monostate {}), expected);
}

#if defined(__cpp_lib_flat_set)
static_assert(__cpp_lib_flat_set >= 202207L);

TEST(HashTestSuite, FlatSetIsHashable)
{
    static_assert(Hashable<std::flat_set<int>>);
    static_assert(!Hashable<std::flat_set<Unhashable>>);

    constexpr u64 expected = 18239313798490686357u;
    EXPECT_EQ(Hash {}(std::flat_set<int> { 1, 2, 3 }), expected);
}
#endif

#if defined(__cpp_lib_flat_map)
static_assert(__cpp_lib_flat_map >= 202207L);

TEST(HashTestSuite, FlatMapIsHashable)
{
    static_assert(Hashable<std::flat_map<std::string, int>>);
    static_assert(!Hashable<std::flat_map<Unhashable, int>>);
    static_assert(!Hashable<std::flat_map<int, Unhashable>>);

    constexpr u64 expected = 13597235531013623295u;
    EXPECT_EQ(Hash {}(std::flat_map<std::string, int> { { "a", 1 }, { "b", 2 } }), expected);
}
#endif

#if defined(__cpp_lib_indirect)
static_assert(__cpp_lib_indirect >= 202502L);

TEST(HashTestSuite, IndirectIsHashable)
{
    static_assert(Hashable<std::indirect<int>>);
    static_assert(!Hashable<std::indirect<Unhashable>>);

    constexpr u64 expected = 1790149455062925670u;
    EXPECT_EQ(Hash {}(std::indirect<int> { 42 }), expected);
}
#endif

} // namespace ka
