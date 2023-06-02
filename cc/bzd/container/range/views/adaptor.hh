#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"

namespace bzd::range {

template <template <class...> class T>
struct Adaptor
{
	template <class... Args>
	[[nodiscard]] constexpr auto operator()(Args&&... args) const noexcept
	{
		return
			[... args = bzd::forward<Args>(args)](auto&& range) { return T(bzd::inPlace, bzd::forward<decltype(range)>(range), args...); };
	}
};

} // namespace bzd::range

/// No need to require the borroedRange here as it is already enforced by the adaptor.
template <bzd::concepts::range Range, class Adaptor>
constexpr auto operator|(Range&& range, Adaptor&& adaptor) noexcept
{
	return adaptor(bzd::forward<Range>(range));
}
