#pragma once

#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/type_traits/iterator/end.hh"
#include "cc/bzd/type_traits/iterator/begin.hh"

namespace bzd::range
{
/// Associate a range to one or multiple scopes
template <class T, class... Args>
[[nodiscard]] constexpr auto associateToScope(T& range, Args&&... args) noexcept
{
	class Range
	{
	public:
		constexpr Range(T& range, Args&&... args) noexcept : range_{range}, args_{bzd::forward<Args>(args)...} {}

		constexpr auto begin() const noexcept
		{
			return bzd::begin(range_);
		}

    	constexpr auto end() const noexcept
		{
			return bzd::end(range_);
		}

	private:
		T& range_;
		bzd::Tuple<Args...> args_;
	};

	return Range{range, bzd::forward<Args>(args)...};
}
}
