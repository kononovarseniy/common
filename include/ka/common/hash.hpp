#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <ka/common/fixed.hpp>

namespace ka
{

class Hasher;
template <typename T>
struct HashImpl;

template <typename T>
concept Hashable = requires(Hasher & hasher, const T & value) { HashImpl<T>::update(hasher, value); };
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

template <std::convertible_to<std::string_view> T>
struct HashImpl<T> final
{
    static void update(Hasher & hasher, const std::string_view value) noexcept
    {
        hasher.update(reinterpret_cast<const u8 *>(value.data()), value.size());
    }
};

template <typename T>
    requires std::is_arithmetic_v<T> || std::is_enum_v<T>
struct HashImpl<T> final
{
    static void update(Hasher & hasher, const T & value) noexcept
    {
        hasher.update(reinterpret_cast<const u8 *>(&value), sizeof(T));
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
