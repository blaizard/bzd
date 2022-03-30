#pragma once

#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"

namespace bzd::range {
/// Associate a range to one or multiple scopes
template <class T, class... Args>
[[nodiscard]] constexpr auto associateScope(T& range, Args&&... args) noexcept
{
	class Range
	{
	public:
		constexpr Range(T& range, Args&&... args) noexcept : range_{range}, args_{inPlace, bzd::forward<Args>(args)...} {}

		constexpr auto begin() const noexcept { return bzd::begin(range_); }

		constexpr auto end() const noexcept { return bzd::end(range_); }

	private:
		T& range_;
		bzd::Tuple<Args...> args_;
	};

	return Range{range, bzd::forward<Args>(args)...};
}
} // namespace bzd::range
