#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/is_trivially_destructible.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

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

// Storage

template <class T>
struct OptionalNonTrivialStorage
{
	using Self = OptionalNonTrivialStorage<T>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	constexpr OptionalNonTrivialStorage() noexcept : isValue_{false}, empty_{} {}

	template <class U>
	constexpr OptionalNonTrivialStorage(U&& value) noexcept : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr OptionalNonTrivialStorage(const Self& value) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept = default;
	constexpr OptionalNonTrivialStorage(Self&& value) noexcept = default;
	constexpr Self& operator=(Self&& optional) noexcept = default;

	~OptionalNonTrivialStorage() noexcept
	{
		if (isValue_) value_.~ValueContainer();
	}

	bool isValue_;
	union {
		bzd::UInt8Type empty_;
		ValueContainer value_;
	};
};

template <class T>
struct OptionalTrivialStorage
{
	using Self = OptionalTrivialStorage<T>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	constexpr OptionalTrivialStorage() noexcept : isValue_{false}, empty_{} {}

	template <class U>
	constexpr OptionalTrivialStorage(U&& value) noexcept : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr OptionalTrivialStorage(const Self& value) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept = default;
	constexpr OptionalTrivialStorage(Self&& value) noexcept = default;
	constexpr Self& operator=(Self&& optional) noexcept = default;
	~OptionalTrivialStorage() noexcept = default;

	bool isValue_{false};
	union {
		bzd::UInt8Type empty_;
		ValueContainer value_;
	};
};

template <class T>
class Optional
{
public:
	using Self = Optional<T>;
	using Value = bzd::typeTraits::RemoveReference<T>;
	using StorageType =
		bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<T>, OptionalTrivialStorage<T>, OptionalNonTrivialStorage<T>>;
	using ValueContainer = typename StorageType::ValueContainer;

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
	constexpr Optional(const OptionalNull& null) noexcept : Optional{} {}

	// Forward constructor to storage type for all non-self typed
	template <class U, typename = typeTraits::EnableIf<!IsSelf<U>::value>>
	constexpr Optional(U&& value) : storage_{bzd::forward<U>(value)}
	{
	}

public: // API
	constexpr bool hasValue() const noexcept { return storage_.isValue_; }
	constexpr explicit operator bool() const noexcept { return hasValue(); }

	constexpr const Value& valueOr(const Value& defaultValue) const noexcept
	{
		return (storage_.isValue_) ? storage_.value_ : defaultValue;
	}

	constexpr const Value& value() const
	{
		bzd::assert::isTrue(storage_.isValue_);
		return storage_.value_;
	}

	constexpr Value& valueMutable()
	{
		bzd::assert::isTrue(storage_.isValue_);
		return storage_.value_;
	}

	constexpr const Value* operator->() const { return &value(); }

	constexpr Value* operator->() { return &valueMutable(); }

	/**
	 * Constructs the contained value in-place.
	 * If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
	 */
	template <class... Args>
	constexpr void emplace(Args&&... args) noexcept
	{
		if (storage_.isValue_)
		{
			storage_.value_.~ValueContainer();
		}
		::new (&(storage_.value_)) ValueContainer(bzd::forward<Args>(args)...);
		storage_.isValue_ = true;
	}

	constexpr void reset() noexcept
	{
		if (storage_.isValue_)
		{
			storage_.value_.~ValueContainer();
		}
		storage_.isValue_ = false;
	}

private:
	StorageType storage_;
};

} // namespace bzd::impl

namespace bzd {

constexpr impl::OptionalNull nullopt = impl::OptionalNull::make();

/**
 * \brief Type managing an optional contained value, i.e. a value that may or
 * may not be present.
 */
template <class T>
using Optional = impl::Optional<T>;
} // namespace bzd
