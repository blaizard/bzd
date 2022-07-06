#pragma once

#include "cc/bzd/container/iterator/bidirectional.hh"

namespace bzd::iterator {

template <class T, class CRTP = void, class Policies = impl::DefaultPolicies<T>>
class RandomAccess : public impl::ClassTraits<RandomAccess, T, CRTP, Policies, Bidirectional>::Parent
{
private:
	using Traits = impl::ClassTraits<RandomAccess, T, CRTP, Policies, Bidirectional>;

public: // Traits
	using Self = typename Traits::Self;
	using ActualSelf = typename Traits::ActualSelf;
	using Parent = typename Traits::Parent;
	using Category = typeTraits::RandomAccessTag;
	using IndexType = typename Policies::IndexType;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;

public: // Constructors
	constexpr RandomAccess(ValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	RandomAccess(const RandomAccess&) = default;
	RandomAccess& operator=(const RandomAccess&) = default;
	RandomAccess(RandomAccess&&) = default;
	RandomAccess& operator=(RandomAccess&&) = default;

public: // API
	[[nodiscard]] constexpr ActualSelf operator+(const int n) const noexcept
	{
		ActualSelf it{static_cast<const ActualSelf&>(*this)};
		Policies::increment(it.data_, n);
		return it;
	}

	[[nodiscard]] constexpr ActualSelf operator-(const int n) const noexcept
	{
		ActualSelf it(static_cast<const ActualSelf&>(*this));
		Policies::decrement(it.data_, n);
		return it;
	}

	constexpr ActualSelf& operator+=(const int n) noexcept
	{
		Policies::increment(this->data_, n);
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf& operator-=(const int n) noexcept
	{
		Policies::decrement(this->data_, n);
		return static_cast<ActualSelf&>(*this);
	}

	[[nodiscard]] constexpr DifferenceType operator-(const Self& other) const noexcept
	{
		return static_cast<DifferenceType>(&(this->operator*()) - &(*other));
	}

	[[nodiscard]] constexpr auto& operator[](const Size index) noexcept { return Policies::at(this->data_, index); }
	[[nodiscard]] constexpr auto& operator[](const Size index) const noexcept { return Policies::at(this->data_, index); }
};
} // namespace bzd::iterator
