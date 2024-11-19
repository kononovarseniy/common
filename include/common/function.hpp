#include <concepts>
#include <functional>

namespace ka
{

namespace detail
{

template <typename F, typename Signature>
constexpr bool function = false;

template <typename F, typename Ret, typename... Args>
constexpr bool function<F, Ret (Args...)> = requires(F && f, Args &&... args) {
	{
		std::invoke(std::forward<F>(f), std::forward<Args>(args)...)
	} -> std::convertible_to<Ret>;
};

template <typename F, typename Ret, typename... Args>
constexpr bool function<F, Ret (Args...) noexcept> = requires(F && f, Args &&... args) {
	{
		std::invoke(std::forward<F>(f), std::forward<Args>(args)...)
	} noexcept -> std::convertible_to<Ret>;
};

} // namespace detail

template <typename F, typename Signature>
concept function = detail::function<F, Signature>;

} // namespace ka
