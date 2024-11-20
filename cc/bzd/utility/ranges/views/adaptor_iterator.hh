#pragma once

#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::ranges {

template <class Range, class CRTP>
class AdapatorForwardIterator : public typeTraits::IteratorBase
{
private:
	using Iterator = typeTraits::RangeIterator<Range>;

public: // Traits
	using ValueType = typeTraits::IteratorValue<Iterator>;
	static constexpr auto category = typeTraits::IteratorCategory::forward;

public:
	constexpr AdapatorForwardIterator(Iterator&& it) noexcept : it_{bzd::move(it)} {}

	constexpr auto& operator++() noexcept
	{
		static_cast<CRTP*>(this)->next(it_);
		return static_cast<CRTP&>(*this);
	}
	[[nodiscard]] constexpr auto operator++(int) noexcept
	{
		CRTP it{static_cast<const CRTP&>(*this)};
		static_cast<CRTP*>(this)->next(it_);
		return it;
	}
	[[nodiscard]] constexpr Bool operator==(const auto& it) const noexcept { return it_ == it; }
	[[nodiscard]] constexpr Bool operator!=(const auto& it) const noexcept { return it_ != it; }
	constexpr auto operator*() const noexcept
	{
		return static_cast<const CRTP*>(this)->get(it_);
		;
	}

public: // Overwritable API.
	constexpr void next(auto& it) noexcept { ++it; }
	constexpr auto get(const auto& it) const noexcept { return *it; }

private:
	Iterator it_;
};

} // namespace bzd::ranges
