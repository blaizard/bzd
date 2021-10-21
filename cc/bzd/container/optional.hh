#pragma once

#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_reference.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/is_trivially_destructible.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/move.hh"

#include <new> // Used only for placement new

namespace bzd::impl {

// nullopt

class OptionalNull
{
public:
	static constexpr OptionalNull make() { return OptionalNull{}; }

private:
	explicit constexpr OptionalNull() noexcept {}
};

template <class T>
class Optional
{
public:
	using Self = Optional<T>;
	using Value = bzd::typeTraits::RemoveReference<T>;
	using ValueContainer = T; // bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	template <class U>
	using IsSelf = bzd::typeTraits::IsSame<bzd::typeTraits::Decay<U>, Self>;

public: // Constructors
	// Default constructor
	explicit constexpr Optional() noexcept = default;

	// Copy/move constructor/assignment
	constexpr Optional(const Self& optional) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept = default;
	constexpr Optional(Self&& optional) noexcept = default;
	constexpr Self& operator=(Self&& optional) noexcept = default;

	// Support for bzd::nullopt
	constexpr Optional(const OptionalNull&) noexcept : Optional{} {}

	// Forward constructor to storage type for all non-self typed
	template <class U, typename = typeTraits::EnableIf<!IsSelf<U>::value>>
	constexpr Optional(U&& value) : data_{bzd::forward<U>(value)}
	{
	}

public: // API
	constexpr bool hasValue() const noexcept { return data_.template is<ValueContainer>(); }
	constexpr explicit operator bool() const noexcept { return hasValue(); }

	constexpr const Value& valueOr(const Value& defaultValue) const noexcept
	{
		return (hasValue()) ? data_.template get<ValueContainer>() : defaultValue;
	}

	constexpr const Value& value() const
	{
		bzd::assert::isTrue(hasValue());
		return data_.template get<ValueContainer>();
	}

	constexpr Value& valueMutable()
	{
		bzd::assert::isTrue(hasValue());
		return data_.template get<ValueContainer>();
	}

	constexpr const Value* operator->() const { return &value(); }

	constexpr Value* operator->() { return &valueMutable(); }

	// Constructs the contained value in-place.
	// If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
	template <class... Args>
	constexpr void emplace(Args&&... args) noexcept
	{
		data_.template emplace<ValueContainer>(bzd::forward<Args>(args)...);
	}

	constexpr void reset() noexcept
	{
		if (hasValue())
		{
			data_ = OptionalNull::make();
		}
	}

private:
	bzd::Variant<OptionalNull, ValueContainer> data_{};
};

} // namespace bzd::impl

namespace bzd {

constexpr impl::OptionalNull nullopt = impl::OptionalNull::make();

// \brief Type managing an optional contained value, i.e. a value that may or
// may not be present.
template <class T>
using Optional = impl::Optional<T>;
} // namespace bzd
