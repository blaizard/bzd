#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/iterators/prev.hh"

namespace bzd::iterator {

template <concepts::bidirectionalIterator Iterator>
class Reverse : public typeTraits::IteratorBase
{
public: // Traits.
	using Self = Reverse;
	using ValueType = typename typeTraits::IteratorValue<Iterator>;
	using DifferenceType = typename typeTraits::IteratorDifference<Iterator>;
	static constexpr auto category = typeTraits::iteratorCategory<Iterator>;

public: // Constructors.
	Reverse() = default;
	constexpr explicit Reverse(const Iterator& it) noexcept : it_(it) {}
	constexpr Reverse(bzd::InPlace, const Iterator& it) noexcept : it_(it) {}

public: // Copy/move constructors/assignments
	Reverse(const Reverse&) = default;
	Reverse& operator=(const Reverse&) = default;
	Reverse(Reverse&&) = default;
	Reverse& operator=(Reverse&&) = default;
	~Reverse() = default;

public: // API.
	[[nodiscard]] constexpr auto& operator*() const noexcept { return *bzd::prev(it_); }

	constexpr Reverse& operator++() noexcept
	{
		--it_;
		return *this;
	}
	[[nodiscard]] constexpr const Reverse& operator++(int) noexcept
	{
		auto tmp = *this;
		++(*this);
		return tmp;
	}

	constexpr Reverse& operator--() noexcept
	{
		++it_;
		return *this;
	}
	[[nodiscard]] constexpr const Reverse& operator--(int) noexcept
	{
		auto tmp = *this;
		--(*this);
		return tmp;
	}

	[[nodiscard]] constexpr Bool operator==(const Self& it) const noexcept { return it_ == it.it_; }
	[[nodiscard]] constexpr Bool operator!=(const Self& it) const noexcept { return !(it == *this); }

protected:
	Iterator it_{};
};

} // namespace bzd::iterator
