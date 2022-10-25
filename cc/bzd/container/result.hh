#pragma once

#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/first_type.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_reference.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_const.hh"
#include "cc/bzd/type_traits/remove_volatile.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"

namespace bzd {
// Forward declaration for the "friend" attribute
template <class T, class E>
class Result;
} // namespace bzd

namespace bzd::impl {

// nullresult

class ResultNull
{
public:
	static constexpr ResultNull make() noexcept { return ResultNull{}; }

private:
	explicit constexpr ResultNull() noexcept {}
};

} // namespace bzd::impl

namespace bzd {

/// Class used to hold an error object, it is the only way to pass errors to a Result object.
template <class E>
class ResultError
{
public:
	template <class... Args>
	constexpr ResultError(Args&&... args) noexcept : error_{bzd::forward<Args>(args)...}
	{
	}

private:
	template <class A, class B>
	friend class bzd::Result;
	E error_;
};

/// \brief This is the type used for returning and propagating errors.
///
/// It is a variants with 2 states, valid, representing success and containing a
/// value, and error, representing error and containing an error value.
template <class T = void, class E = bzd::Bool>
class Result
{
public: // Traits
	// The container type for the value.
	using ValueContainer = typeTraits::Conditional<typeTraits::isSame<T, void>, void*, T>;
	// Value type returned.
	using Value = typeTraits::RemoveVolatile<typeTraits::RemoveReference<ValueContainer>>;
	// Error type returned.
	using Error = typeTraits::RemoveVolatile<typeTraits::RemoveReference<E>>;
	// The container type for the error.
	using ErrorContainer = ResultError<Error>;
	// The current type.
	using Self = Result<T, E>;
	template <class U>
	using IsSelf = bzd::typeTraits::IsSame<bzd::typeTraits::Decay<U>, Self>;

public: // Constructors
	// Copy/move constructor/assignment.
	constexpr Result(const Self& result) noexcept = default;
	constexpr Self& operator=(const Self& result) noexcept = default;
	constexpr Result(Self&& result) noexcept = default;
	constexpr Self& operator=(Self&& result) noexcept = default;

	// Support for bzd::nullresult.
	constexpr Result(const impl::ResultNull&) noexcept : data_{bzd::inPlaceType<ValueContainer>, nullptr} {}

	// Forwards arguments to construct the value to the storage type.
	template <class... Args>
	requires(!IsSelf<typeTraits::FirstType<Args...>>::value &&
			 !typeTraits::isBaseOf<ResultError<E>, typeTraits::FirstType<Args...>>) constexpr Result(Args&&... args) noexcept :
		data_{bzd::inPlaceType<ValueContainer>, bzd::forward<Args>(args)...}
	{
	}

	// Construct an error result.
	template <class U>
	constexpr Result(ResultError<U>&& error) noexcept : data_{bzd::inPlaceType<ErrorContainer>, bzd::move(error.error_)}
	{
	}

	// Value assignment is forbidden on reference type. This is because it is confusing whether the
	// existing reference updates its value or the reference gets re-assigned.
	template <class U>
	requires(!IsSelf<U>::value && concepts::reference<T>) constexpr Self& operator=(U) noexcept = delete;

public: // API
	/// Checks whether the result contains a value.
	///
	/// \return true if *this contains a value, false if *this does not contain a value.
	[[nodiscard]] constexpr bool hasValue() const noexcept { return data_.template is<ValueContainer>(); }

	/// Checks whether the result contains an error.
	///
	/// \return true if *this contains an error, false if *this does not contain an error.
	[[nodiscard]] constexpr bool hasError() const noexcept { return data_.template is<ErrorContainer>(); }

	/// \copydoc hasValue
	[[nodiscard]] constexpr explicit operator bool() const noexcept { return hasValue(); }

	/// Returns the contained value if the result has a value, otherwise returns defaultValue.
	///
	/// \param defaultValue The value to use in case *this is empty.
	/// \return The current value if *this has a value, or defaultValue otherwise.
	template <class DefaultValue>
	[[nodiscard]] constexpr const Value& valueOr(DefaultValue&& defaultValue) const noexcept
	{
		static_assert(concepts::lValueReference<DefaultValue> && concepts::sameAs<typeTraits::RemoveConst<DefaultValue>, Value&>,
					  "The default value will become a dangling reference, you should use 'valueOrAsCopy' instead.");
		return (hasValue()) ? data_.template get<ValueContainer>() : defaultValue;
	}

	/// Returns a copy of the contained value if the result has a value, otherwise returns defaultValue.
	///
	/// \param defaultValue The value to use in case *this is empty.
	/// \return The current value if *this has a value, or defaultValue otherwise.
	[[nodiscard]] constexpr Value valueOrAsCopy(const Value& defaultValue) const noexcept { return valueOr(defaultValue); }

	/// If *this contains a value, returns a const reference to the contained value otherwise, asserts.
	///
	/// \return A const reference to the contained value.
	[[nodiscard]] constexpr const Value& value() const noexcept
	{
		bzd::assert::isTrue(hasValue());
		return data_.template get<ValueContainer>();
	}

	/// If *this contains a value, returns a reference to the contained value otherwise, asserts.
	///
	/// \return A reference to the contained value.
	[[nodiscard]] constexpr Value& valueMutable() noexcept
	{
		bzd::assert::isTrue(hasValue());
		return data_.template get<ValueContainer>();
	}

	/// If *this contains an error, returns a const reference to the contained error otherwise, asserts.
	///
	/// \return A const reference to the contained error.
	[[nodiscard]] constexpr const Error& error() const noexcept
	{
		bzd::assert::isTrue(hasError());
		return data_.template get<ErrorContainer>().error_;
	}

	/// If *this contains an error, returns a mutable reference to the contained error otherwise, asserts.
	///
	/// \return A reference to the contained error.
	[[nodiscard]] constexpr Error& errorMutable() noexcept
	{
		bzd::assert::isTrue(hasError());
		return data_.template get<ErrorContainer>().error_;
	}

	/// Propagate the error to another result object.
	///
	/// \return The error object packed to be consumed by a compatible result object.
	[[nodiscard]] constexpr ResultError<Error> propagate() && noexcept
	{
		bzd::assert::isTrue(hasError());
		return bzd::move(data_.template get<ErrorContainer>().error_);
	}

	/// Accesses the contained value.
	///
	/// \return Returns a pointer to the contained value.
	[[nodiscard]] constexpr const Value* operator->() const noexcept { return &value(); }

	/// Accesses the contained value.
	///
	/// \return Returns a pointer to the contained value.
	[[nodiscard]] constexpr Value* operator->() noexcept { return &valueMutable(); }

	/// Constructs the contained value in-place.
	///
	/// \param args... Arguments to be passed to the constructor.
	template <class... Args>
	constexpr void emplace(Args&&... args) noexcept
	{
		data_.template emplace<ValueContainer>(bzd::forward<Args>(args)...);
	}

private:
	bzd::Variant<ErrorContainer, ValueContainer> data_{};
};

constexpr impl::ResultNull nullresult = impl::ResultNull::make();

} // namespace bzd

namespace bzd::error {
/// Create an error from a specific type.
template <class E = bzd::Bool>
constexpr ResultError<E> make(E&& e = true) noexcept
{
	return ResultError<E>(bzd::forward<E>(e));
}
} // namespace bzd::error
