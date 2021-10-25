#pragma once

#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_reference.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"

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
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;
	template <class U>
	using IsSelf = bzd::typeTraits::IsSame<bzd::typeTraits::Decay<U>, Self>;

private:
	struct OptionalEmptyType
	{
	};

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
	constexpr Optional(U&& value) : data_{bzd::inPlaceType<ValueContainer>, bzd::forward<U>(value)}
	{
	}

	// Value assignment is forbidden on reference type. This is because it is confusing whether the
	// existing reference updates its value or the reference gets re-assigned.
	template <class U, typename = typeTraits::EnableIf<!IsSelf<U>::value && bzd::typeTraits::isReference<T>>>
	constexpr Self& operator=(U) noexcept = delete;

public: // API
	/// Checks whether the optional contains a value.
	///
	/// \return true if *this contains a value, false if *this does not contain a value.
	[[nodiscard]] constexpr bool hasValue() const noexcept { return data_.template is<ValueContainer>(); }

	/// \copydoc hasValue
	[[nodiscard]] constexpr explicit operator bool() const noexcept { return hasValue(); }

	/// Returns the contained value if the optional has a value, otherwise returns defaultValue.
	///
	/// \param defaultValue The value to use in case *this is empty.
	/// \return The current value if *this has a value, or default_value otherwise.
	[[nodiscard]] constexpr const Value& valueOr(const Value& defaultValue) const noexcept
	{
		return (hasValue()) ? data_.template get<ValueContainer>() : defaultValue;
	}

	/// If *this contains a value, returns a const reference to the contained value otherwise, asserts.
	///
	/// \return A const reference to the contained value.
	[[nodiscard]] constexpr const Value& value() const
	{
		bzd::assert::isTrue(hasValue());
		return data_.template get<ValueContainer>();
	}

	/// If *this contains a value, returns a reference to the contained value otherwise, asserts.
	///
	/// \return A reference to the contained value.
	[[nodiscard]] constexpr Value& valueMutable()
	{
		bzd::assert::isTrue(hasValue());
		return data_.template get<ValueContainer>();
	}

	/// Accesses the contained value.
	///
	/// \return Returns a pointer to the contained value.
	[[nodiscard]] constexpr const Value* operator->() const { return &value(); }

	/// Accesses the contained value.
	///
	/// \return Returns a pointer to the contained value.
	[[nodiscard]] constexpr Value* operator->() { return &valueMutable(); }

	/// Constructs the contained value in-place.
	///
	/// \param args... Arguments to be passed to the constructor.
	template <class... Args>
	constexpr void emplace(Args&&... args) noexcept
	{
		data_.template emplace<ValueContainer>(bzd::forward<Args>(args)...);
	}

	/// Destroys any contained value.
	///
	/// If *this contains a value, destroy that value. Otherwise, there are no effects.
	constexpr void reset() noexcept
	{
		if (hasValue())
		{
			data_ = OptionalEmptyType{};
		}
	}

private:
	bzd::Variant<OptionalEmptyType, ValueContainer> data_{};
};

} // namespace bzd::impl

namespace bzd {

constexpr impl::OptionalNull nullopt = impl::OptionalNull::make();

// \brief Type managing an optional contained value, i.e. a value that may or
// may not be present.
template <class T>
using Optional = impl::Optional<T>;
} // namespace bzd
