#pragma once

#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/first_type.hh"
#include "cc/bzd/type_traits/invocable.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_reference.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/remove_volatile.hh"
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

} // namespace bzd::impl

namespace bzd {

// \brief Type managing an optional contained value, i.e. a value that may or
// may not be present.
template <class T>
class Optional
{
public:
	using Self = Optional<T>;
	using Value = typeTraits::RemoveVolatile<typeTraits::RemoveReference<T>>;
	using ValueContainer = T;
	template <class U>
	using IsSelf = bzd::typeTraits::IsSame<bzd::typeTraits::Decay<U>, Self>;

private:
	struct OptionalEmptyType
	{
		[[nodiscard]] constexpr Bool operator==(const OptionalEmptyType&) const noexcept { return true; }
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
	constexpr Optional(const impl::OptionalNull&) noexcept : Optional{} {}

	// Forward constructor to storage type for all non-self typed
	template <class... Args>
	requires(!IsSelf<typeTraits::FirstType<Args...>>::value)
	constexpr Optional(Args&&... args) noexcept : data_{bzd::inPlaceType<ValueContainer>, bzd::forward<Args>(args)...}
	{
	}

	// Value assignment is forbidden on reference type. This is because it is confusing whether the
	// existing reference updates its value or the reference gets re-assigned.
	template <class U>
	requires(!IsSelf<U>::value && concepts::reference<T>)
	constexpr Self& operator=(U) noexcept = delete;

public: // Comparators.
	[[nodiscard]] constexpr Bool operator==(const Self& other) const noexcept { return (data_ == other.data_); }
	[[nodiscard]] constexpr Bool operator!=(const Self& other) const noexcept { return !(*this == other); }

public: // API
	/// Checks whether the optional contains a value.
	///
	/// \return true if *this contains a value, false if *this does not contain a value.
	[[nodiscard]] constexpr bool hasValue() const noexcept { return data_.template is<ValueContainer>(); }

	/// Checks whether the optional is empty (does not contain a value).
	///
	/// \return true if *this is empty, false if *this contains a value.
	[[nodiscard]] constexpr bool empty() const noexcept { return !hasValue(); }

	/// \copydoc hasValue
	[[nodiscard]] constexpr explicit operator bool() const noexcept { return hasValue(); }

	/// Returns the contained value if the optional has a value, otherwise returns defaultValue.
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

	/// Returns of copy of the contained value if the optional has a value, otherwise returns defaultValue.
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

public: // Monadic API.
	/// If the optional contains a value, invokes 'callable' with the contained value as an argument,
	/// and returns the result of that invocation; otherwise, returns an empty optional.
	///
	/// \param callable A suitable function or Callable object that returns an optional.
	/// \return The result of 'callable' or an empty optional.
	template <concepts::invocable<const T&> Callable>
	constexpr auto andThen(Callable&& callable) && noexcept
	{
		if (hasValue())
		{
			return bzd::forward<Callable>(callable)(valueMutable());
		}
		using ReturnType = typeTraits::InvokeResult<Callable, const T&>;
		if constexpr (!concepts::sameAs<ReturnType, void>)
		{
			return ReturnType{};
		}
	}

	/// Returns *this if it contains a value. Otherwise, returns the result of `callable`.
	///
	/// \param callable A suitable function or Callable object that returns an optional.
	/// \return The result of 'callable' or an empty optional.
	template <concepts::invocable<> Callable>
	requires(concepts::sameClassAs<typeTraits::InvokeResult<Callable>, Optional>)
	constexpr Optional orElse(Callable&& callable) && noexcept
	{
		if (hasValue())
		{
			return *this;
		}
		return bzd::forward<Callable>(callable)();
	}

private:
	bzd::Variant<OptionalEmptyType, ValueContainer> data_{};
};

template <class T>
Optional(T&&) -> Optional<T>;

constexpr impl::OptionalNull nullopt = impl::OptionalNull::make();

} // namespace bzd
