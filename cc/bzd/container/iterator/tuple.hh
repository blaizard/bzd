#pragma once

#include "cc/bzd/container/variant.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_reference.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

template <class ContainerType, class... Ts>
class Tuple
{
public: // Traits
	using Self = Tuple<ContainerType, Ts...>;
	using Category = typeTraits::RandomAccessTag;
	using IndexType = bzd::Size;
	using DifferenceType = bzd::Int32;
	using ValueType = bzd::Variant<bzd::typeTraits::AddReference<Ts>...>;

public: // Constructors
	constexpr Tuple(ContainerType& tuple) noexcept : tuple_{tuple}, index_{0}, value_{bzd::inPlaceIndex<0>, tuple_.template get<0>()} {}

public: // Copy/move constructors/assignments
	constexpr Tuple(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Tuple(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

public: // API
	constexpr Self& operator++() noexcept
	{
		++index_;
		updateValue();
		return *this;
	}

	constexpr Self operator++(int) noexcept
	{
		Self it{*this};
		++(*this);
		return it;
	}

	constexpr Self& operator--() noexcept
	{
		--index_;
		updateValue();
		return *this;
	}

	constexpr Self operator--(int) noexcept
	{
		Self it{*this};
		--(*this);
		return it;
	}

	[[nodiscard]] constexpr Self operator-(const int n) const noexcept
	{
		Self it{*this};
		it -= n;
		return it;
	}

	[[nodiscard]] constexpr Self operator+(const int n) const noexcept
	{
		Self it{*this};
		it += n;
		return it;
	}

	constexpr Self& operator-=(const int n) noexcept
	{
		index_ -= n;
		updateValue();
		return *this;
	}

	constexpr Self& operator+=(const int n) noexcept
	{
		index_ += n;
		updateValue();
		return *this;
	}

	[[nodiscard]] constexpr auto& operator[](const Size index) noexcept
	{
		Self it{*this};
		it += index;
		it->updateValue();
		return *it;
	}

	[[nodiscard]] constexpr auto& operator[](const Size index) const noexcept
	{
		Self it{*this};
		it += index;
		it->updateValue();
		return *it;
	}

	[[nodiscard]] constexpr Bool operator==(const Self& it) const noexcept { return (it.index_ == index_) && (&(it.tuple_) == &(tuple_)); }
	[[nodiscard]] constexpr Bool operator!=(const Self& it) const noexcept { return !(it == *this); }

	[[nodiscard]] constexpr ValueType& operator*() noexcept { return value_; }
	[[nodiscard]] constexpr ValueType* operator->() noexcept { return &value_; }

private:
	template <bzd::Size current = 0>
	constexpr void updateValue() noexcept
	{
		if (index_ == current)
		{
			value_.template emplace<current>(tuple_.template get<current>());
		}
		else if constexpr (current + 1 < ContainerType::size())
		{
			updateValue<current + 1>();
		}
	}

private:
	ContainerType& tuple_;
	bzd::Size index_;
	ValueType value_;
};

} // namespace bzd::iterator
