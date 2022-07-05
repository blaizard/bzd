#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

template <class T>
struct ForwardDefaultPolicies
{
	using ValueType = T;
	using IndexType = bzd::Size;
	using DifferenceType = bzd::Int32;

	static constexpr void increment(T*& data) noexcept { ++data; }
};

template <class T, class CRTP = void, class Policies = ForwardDefaultPolicies<T>>
class Forward : public typeTraits::IteratorBase
{
public: // Traits
	using Self = Forward<T, CRTP, Policies>;
	using ActualSelf = typeTraits::Conditional<typeTraits::isSame<CRTP, void>, Self, CRTP>;
	using Category = typeTraits::ForwardTag;
	using IndexType = typename Policies::IndexType;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;

public: // Constructors
	constexpr Forward(ValueType* data) noexcept : data_{data} {}

public: // Copy/move constructors/assignments
	Forward(const Self&) = default;
	Self& operator=(const Self&) = default;
	Forward(Self&&) = default;
	Self& operator=(Self&&) = default;

public: // API
	constexpr ActualSelf& operator++() noexcept
	{
		Policies::increment(data_);
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf operator++(int) noexcept
	{
		ActualSelf it{static_cast<const ActualSelf&>(*this)};
		Policies::increment(data_);
		return it;
	}

	[[nodiscard]] constexpr Bool operator==(const Self& it) const noexcept { return it.data_ == data_; }

	[[nodiscard]] constexpr Bool operator!=(const Self& it) const noexcept { return !(it == *this); }

	[[nodiscard]] constexpr ValueType& operator*() const noexcept { return *data_; }

	[[nodiscard]] constexpr ValueType* operator->() const noexcept { return data_; }

protected:
	ValueType* data_{nullptr};
};
} // namespace bzd::iterator
