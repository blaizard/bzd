#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

namespace impl {

template <template <class, class, class> class SelfClass,
		  class T,
		  class CRTP,
		  class Policies,
		  template <class, class, class> class ParentClass = SelfClass>
struct ClassTraits
{
	using Self = SelfClass<T, CRTP, Policies>;
	using ActualSelf = typeTraits::Conditional<typeTraits::isSame<CRTP, void>, Self, CRTP>;
	using Parent = ParentClass<T, ActualSelf, Policies>;
};

template <class T>
struct DefaultPolicies
{
	using ValueType = T;
	using IndexType = bzd::Size;
	using DifferenceType = bzd::Int32;

	static constexpr void increment(T*& data) noexcept { ++data; }
	static constexpr void decrement(T*& data) noexcept { --data; }
	static constexpr void increment(T*& data, const int n) noexcept { data += n; }
	static constexpr void decrement(T*& data, const int n) noexcept { data -= n; }
	static constexpr auto& at(auto* data, const Size n) noexcept { return data[n]; }
};

} // namespace impl

template <class T, class CRTP = void, class Policies = impl::DefaultPolicies<T>>
class Forward : public typeTraits::IteratorBase
{
private:
	using Traits = impl::ClassTraits<Forward, T, CRTP, Policies>;

public: // Traits
	using Self = typename Traits::Self;
	using ActualSelf = typename Traits::ActualSelf;
	using IndexType = typename Policies::IndexType;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;
	static constexpr auto category = typeTraits::IteratorCategory::forward;

public: // Constructors
	constexpr Forward(ValueType* data) noexcept : data_{data} {}

public: // Copy/move constructors/assignments
	Forward(const Forward&) = default;
	Forward& operator=(const Forward&) = default;
	Forward(Forward&&) = default;
	Forward& operator=(Forward&&) = default;
	~Forward() = default;

public: // API
	constexpr ActualSelf& operator++() noexcept
	{
		Policies::increment(data_);
		return static_cast<ActualSelf&>(*this);
	}

	[[nodiscard]] constexpr ActualSelf operator++(int) noexcept
	{
		ActualSelf it{static_cast<const ActualSelf&>(*this)};
		Policies::increment(data_);
		return it;
	}

	[[nodiscard]] constexpr Bool operator==(const Self& it) const noexcept { return it.data_ == data_; }

	[[nodiscard]] constexpr Bool operator!=(const Self& it) const noexcept { return !(it == *this); }

	[[nodiscard]] constexpr ValueType& operator*() const noexcept { return Policies::at(data_, 0); }

	[[nodiscard]] constexpr ValueType* operator->() const noexcept { return &Policies::at(data_, 0); }

protected:
	ValueType* data_{nullptr};
};
} // namespace bzd::iterator
