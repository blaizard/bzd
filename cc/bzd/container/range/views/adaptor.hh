#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::range {

template <template <class...> class T>
struct Adaptor
{
	template <class... Args>
	[[nodiscard]] constexpr auto operator()(Args&&... args) const noexcept
	{
		return [... args = bzd::forward<Args>(args)](auto& Range) { return T(Range, args...); };
	}
};

} // namespace bzd::range

template <bzd::concepts::range Range, class Adaptor>
constexpr auto operator|(Range&& range, Adaptor&& adaptor) noexcept
{
	return adaptor(range);
}
