#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_arithmetic.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::test {

template <concepts::arithmetic T>
bzd::Generator<T> generator(const T end) noexcept
{
	for (T value = static_cast<T>(0); value < end; ++value)
	{
		co_yield value;
	}
}

template <concepts::arithmetic T>
bzd::Generator<T> generator(const T start, const T end, const T increment = static_cast<T>(1)) noexcept
{
	for (T value = start; value < end; value += increment)
	{
		co_yield value;
	}
}

template <concepts::syncRange Range>
bzd::Generator<typeTraits::RemoveCVRef<typeTraits::RangeValue<Range>>> generator(Range&& range) noexcept
{
	for (const auto& value : range)
	{
		co_yield value;
	}
}

} // namespace bzd::test
