#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sized_sentinel_for.hh"

namespace bzd::range {

template <class Range>
class View
{
public:
	constexpr Bool empty() const noexcept { return (child().begin() == child().end()); }

	constexpr auto size() const noexcept
	{
		static_assert(concepts::forwardRange<Range>);
		static_assert(concepts::sizedSentinelFor<typename typeTraits::Range<Range>::Iterator, typename typeTraits::Range<Range>::Sentinel>);
		return child().end() - child().begin();
	}

	constexpr const auto* data() const noexcept
	{
		static_assert(concepts::contiguousRange<Range>);
		return &(*(child().begin()));
	}

	constexpr auto* data() noexcept
	{
		static_assert(concepts::contiguousRange<Range>);
		return &(*(child().begin()));
	}

	constexpr auto& operator[](const Size index) noexcept
	{
		static_assert(concepts::randomAccessRange<Range>);
		return *(child().begin() + index);
	}
	constexpr const auto& operator[](const Size index) const noexcept
	{
		static_assert(concepts::randomAccessRange<Range>);
		return *(child().begin() + index);
	}

private:
	Range& child() noexcept { return *static_cast<Range*>(this); }
	const Range& child() const noexcept { return *static_cast<const Range*>(this); }
};

} // namespace bzd::range
