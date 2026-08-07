#pragma once

#include <bitset>
#include <concepts>
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
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#if defined(__has_include)
    #if __has_include(<flat_set>)
        #include <flat_set>
    #endif
    #if __has_include(<flat_map>)
        #include <flat_map>
    #endif
#endif

#include <ka/common/fixed.hpp>

namespace ka
{

class Hasher;
template <typename T>
struct HashImpl;

template <typename T>
concept Hashable = requires(Hasher & hasher, const std::remove_cvref_t<T> & value) {
    HashImpl<std::remove_cvref_t<T>>::update(hasher, value);
};
//! Types which declare method hash(Hasher &) are hashable.
template <typename T>
concept HashableByMethod = requires(Hasher & hasher, const T & value) { value.hash(hasher); };

//! Fowler-Noll-Vo 1a 64-bit.
class Hasher final
{
public:
    Hasher() noexcept = default;

    void update(const u8 * const data, const size_t size) noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            hash_ = (data[i] ^ hash_) * fnv_prime;
        }
    }

    template <Hashable T>
    void update(const T & value) noexcept
    {
        HashImpl<T>::update(*this, value);
    }

    [[nodiscard]] u64 digest() const noexcept
    {
        return hash_;
    }

private:
    constexpr static u64 fnv_offset_basis = 0xcbf29ce484222325;
    constexpr static u64 fnv_prime = 0x00000100000001b3;

private:
    u64 hash_ = fnv_offset_basis;
};

struct Hash final
{
    [[nodiscard]] u64 operator()(const Hashable auto & value) const noexcept
    {
        Hasher hasher;
        hasher.update(value);
        return hasher.digest();
    }
};

template <HashableByMethod T>
struct HashImpl<T> final
{
    static void update(Hasher & hasher, const T & value) noexcept
    {
        value.hash(hasher);
    }
};

template <std::convertible_to<std::string_view> T>
    requires(!std::is_pointer_v<T>)
struct HashImpl<T> final
{
    static void update(Hasher & hasher, const std::string_view value) noexcept
    {
        hasher.update(reinterpret_cast<const u8 *>(value.data()), value.size());
    }
};

// Arithmetic types and enums are basic hashable types.

template <typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
struct HashImpl<T> final
{
    static void update(Hasher & hasher, const T & value) noexcept
    {
        hasher.update(reinterpret_cast<const u8 *>(&value), sizeof(T));
    }
};

// Pointers are hashed by address.

template <typename T>
struct HashImpl<T *> final
{
    static void update(Hasher & hasher, const T * const value) noexcept
    {
        hasher.update(reinterpret_cast<std::uintptr_t>(value));
    }
};

template <typename T, typename Deleter>
struct HashImpl<std::unique_ptr<T, Deleter>> final
{
    static void update(Hasher & hasher, const std::unique_ptr<T, Deleter> & value) noexcept
    {
        hasher.update(reinterpret_cast<std::uintptr_t>(value.get()));
    }
};

template <typename T>
struct HashImpl<std::shared_ptr<T>> final
{
    static void update(Hasher & hasher, const std::shared_ptr<T> & value) noexcept
    {
        hasher.update(reinterpret_cast<std::uintptr_t>(value.get()));
    }
};

// References and pointers with value semantics are hashed by their values.

template <Hashable T>
struct HashImpl<std::reference_wrapper<T>> final
{
    static void update(Hasher & hasher, const std::reference_wrapper<T> value) noexcept
    {
        hasher.update(value.get());
    }
};

#if defined(__cpp_lib_indirect)
template <Hashable T, typename Alloc>
struct HashImpl<std::indirect<T, Alloc>> final
{
    static void update(Hasher & hasher, const std::indirect<T, Alloc> & value) noexcept
    {
        hasher.update(!value.valueless_after_move());
        if (!value.valueless_after_move())
        {
            hasher.update(*value);
        }
    }
};
#endif

//! Sequence containers and associative containers are hashed by their elements in iteration order.

template <Hashable A, Hashable B>
struct HashImpl<std::pair<A, B>> final
{
    static void update(Hasher & hasher, const std::pair<A, B> & value) noexcept
    {
        hasher.update(value.first);
        hasher.update(value.second);
    }
};

template <Hashable... Ts>
struct HashImpl<std::tuple<Ts...>> final
{
    static void update(Hasher & hasher, const std::tuple<Ts...> & value) noexcept
    {
        std::apply(
            [&hasher](const auto &... elements)
            {
                (hasher.update(elements), ...);
            },
            value);
    }
};

template <Hashable T, typename Alloc>
struct HashImpl<std::vector<T, Alloc>> final
{
    static void update(Hasher & hasher, const std::vector<T, Alloc> & value) noexcept
    {
        for (const T & element : value)
        {
            hasher.update(element);
        }
    }
};

template <std::size_t N>
struct HashImpl<std::bitset<N>> final
{
    static void update(Hasher & hasher, const std::bitset<N> & value) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            hasher.update(value[i]);
        }
    }
};

template <Hashable T, typename Compare, typename Alloc>
struct HashImpl<std::set<T, Compare, Alloc>> final
{
    static void update(Hasher & hasher, const std::set<T, Compare, Alloc> & value) noexcept
    {
        for (const auto & element : value)
        {
            hasher.update(element);
        }
    }
};

template <Hashable T, typename Hash, typename KeyEqual, typename Alloc>
struct HashImpl<std::unordered_set<T, Hash, KeyEqual, Alloc>> final
{
    static void update(Hasher & hasher, const std::unordered_set<T, Hash, KeyEqual, Alloc> & value) noexcept
    {
        for (const auto & element : value)
        {
            hasher.update(element);
        }
    }
};

#if defined(__cpp_lib_flat_set)
template <Hashable Key, typename Compare, typename KeyContainer>
struct HashImpl<std::flat_set<Key, Compare, KeyContainer>> final
{
    static void update(Hasher & hasher, const std::flat_set<Key, Compare, KeyContainer> & value) noexcept
    {
        for (const auto & element : value)
        {
            hasher.update(element);
        }
    }
};
#endif

template <Hashable Key, Hashable T, typename Compare, typename Alloc>
struct HashImpl<std::map<Key, T, Compare, Alloc>> final
{
    static void update(Hasher & hasher, const std::map<Key, T, Compare, Alloc> & value) noexcept
    {
        for (const auto & pair : value)
        {
            hasher.update(pair);
        }
    }
};

template <Hashable Key, Hashable T, typename Hash, typename KeyEqual, typename Alloc>
struct HashImpl<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> final
{
    static void update(Hasher & hasher, const std::unordered_map<Key, T, Hash, KeyEqual, Alloc> & value) noexcept
    {
        for (const auto & pair : value)
        {
            hasher.update(pair);
        }
    }
};

#if defined(__cpp_lib_flat_map)
template <Hashable Key, Hashable T, typename Compare, typename KeyContainer, typename MappedContainer>
struct HashImpl<std::flat_map<Key, T, Compare, KeyContainer, MappedContainer>> final
{
    static void update(
        Hasher & hasher,
        const std::flat_map<Key, T, Compare, KeyContainer, MappedContainer> & value) noexcept
    {
        for (const auto & pair : value)
        {
            hasher.update(pair);
        }
    }
};
#endif

template <Hashable T, typename Alloc>
struct HashImpl<std::deque<T, Alloc>> final
{
    static void update(Hasher & hasher, const std::deque<T, Alloc> & value) noexcept
    {
        for (const auto & element : value)
        {
            hasher.update(element);
        }
    }
};

template <Hashable T, typename Alloc>
struct HashImpl<std::list<T, Alloc>> final
{
    static void update(Hasher & hasher, const std::list<T, Alloc> & value) noexcept
    {
        for (const auto & element : value)
        {
            hasher.update(element);
        }
    }
};

template <Hashable T, typename Alloc>
struct HashImpl<std::forward_list<T, Alloc>> final
{
    static void update(Hasher & hasher, const std::forward_list<T, Alloc> & value) noexcept
    {
        for (const auto & element : value)
        {
            hasher.update(element);
        }
    }
};

template <Hashable T>
struct HashImpl<std::optional<T>> final
{
    static void update(Hasher & hasher, const std::optional<T> & value) noexcept
    {
        hasher.update(value.has_value());
        if (value.has_value())
        {
            hasher.update(*value);
        }
    }
};

template <>
struct HashImpl<std::monostate> final
{
    static void update(Hasher &, const std::monostate &) noexcept
    {
    }
};

template <Hashable... Ts>
struct HashImpl<std::variant<Ts...>> final
{
    static void update(Hasher & hasher, const std::variant<Ts...> & value) noexcept
    {
        hasher.update(value.index());
        std::visit(
            [&hasher](const auto & alternative)
            {
                hasher.update(alternative);
            },
            value);
    }
};

struct StrHash final
{
    using is_transparent = void;

    template <std::convertible_to<std::string_view> T>
    [[nodiscard]] u64 operator()(const T & value) const noexcept(noexcept(std::string_view { value }))
    {
        return Hash {}(std::string_view { value });
    }
};

struct StrEq final
{
    using is_transparent = void;

    template <std::convertible_to<std::string_view> A, std::convertible_to<std::string_view> B>
    [[nodiscard]] constexpr auto operator()(A && a, B && b) const
        noexcept(noexcept(std::string_view { std::forward<A>(a) } == std::string_view { std::forward<B>(b) }))
    {
        return std::string_view { std::forward<A>(a) } == std::string_view { std::forward<B>(b) };
    }
};

} // namespace ka
