#pragma once

#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sized_sentinel_for.hh"

namespace bzd::ranges {

template <class Range>
class ViewInterface
{
private:
	// Trick to ensure the concept is not evaluated when Range is still an incomplete type.
	// This waits until the type is actually used to be evaluated.
	template <Bool B = true>
	using CRTP = bzd::typeTraits::Conditional<B, Range, void>;

public:
	constexpr Bool empty() const noexcept { return (child().begin() == child().end()); }

	template <Bool True = true>
	requires(True && concepts::sizedSentinelFor<typeTraits::RangeIterator<CRTP<True>>, typeTraits::RangeSentinel<CRTP<True>>>)
	constexpr auto size() const noexcept
	{
		return child().end() - child().begin();
	}

	template <Bool True = true>
	requires(True && concepts::contiguousRange<CRTP<True>>)
	constexpr const auto* data() const noexcept
	{
		return &(*(child().begin()));
	}

	template <Bool True = true>
	requires(True && concepts::contiguousRange<CRTP<True>>)
	constexpr auto* data() noexcept
	{
		return &(*(child().begin()));
	}

	template <Bool True = true>
	requires(True && concepts::randomAccessRange<CRTP<True>>)
	constexpr auto& operator[](const Size index) noexcept
	{
		return *(child().begin() + index);
	}

	template <Bool True = true>
	requires(True && concepts::randomAccessRange<CRTP<True>>)
	constexpr const auto& operator[](const Size index) const noexcept
	{
		return *(child().begin() + index);
	}

private:
	Range& child() noexcept { return *static_cast<Range*>(this); }
	const Range& child() const noexcept { return *static_cast<const Range*>(this); }
};

} // namespace bzd::ranges

namespace bzd::concepts {

template <class T>
concept view = typeTraits::isBaseOf<bzd::ranges::ViewInterface<typeTraits::RemoveCVRef<T>>, typeTraits::RemoveCVRef<T>>;

}
