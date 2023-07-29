#pragma once

#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/type_traits/is_rvalue_reference.hh"
#include "cc/bzd/utility/ranges/begin.hh"
#include "cc/bzd/utility/ranges/end.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"

namespace bzd::ranges {

namespace impl {
template <class T, class... Args>
class AssociateScopeRange : public ViewInterface<AssociateScopeRange<T, Args...>>
{
public:
	template <class TFwd, class... ArgsFwd>
	constexpr AssociateScopeRange(TFwd&& range, ArgsFwd&&... args) noexcept :
		range_{bzd::forward<TFwd>(range)}, args_{inPlace, bzd::forward<ArgsFwd>(args)...}
	{
	}

public:
	constexpr auto begin() const noexcept { return bzd::begin(range_); }
	constexpr auto end() const noexcept { return bzd::end(range_); }
	constexpr auto size() const noexcept
	requires(concepts::size<T>)
	{
		return bzd::size(range_);
	}

private:
	T range_;
	bzd::Tuple<Args...> args_;
};
} // namespace impl

/// Associate a range to one or multiple scopes.
/// The range is either passed as a reference or moved to the new range.
template <class T, class... Args>
[[nodiscard]] constexpr auto associateScope(T&& range, Args&&... args) noexcept
{
	if constexpr (concepts::rValueReference<T&&>)
	{
		return impl::AssociateScopeRange<typeTraits::RemoveReference<T>, Args...>{bzd::move(range), bzd::forward<Args>(args)...};
	}
	else
	{
		return impl::AssociateScopeRange<T&, Args...>{range, bzd::forward<Args>(args)...};
	}
}

} // namespace bzd::ranges
