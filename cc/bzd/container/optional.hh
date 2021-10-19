#pragma once

#include "cc/bzd/container/reference_wrapper.hh"
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

// Storage

template <class T>
struct OptionalNonTrivialStorage
{
	using Self = OptionalNonTrivialStorage<T>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	constexpr OptionalNonTrivialStorage() noexcept : isValue_{false}, empty_{} {}

	template <class U>
	constexpr OptionalNonTrivialStorage(U&& value, const bool) noexcept : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr OptionalNonTrivialStorage(const Self& optional) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept = default;
	constexpr OptionalNonTrivialStorage(Self&& optional) noexcept = default;
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
	constexpr OptionalTrivialStorage(U&& value, const bool) noexcept : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr OptionalTrivialStorage(const Self& optional) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept
	{
		isValue_ = optional.isValue_;
		if (isValue_)
		{
			value_ = optional.value_;
		}
		return *this;
	}
	constexpr OptionalTrivialStorage(Self&& optional) noexcept = default;
	constexpr Self& operator=(Self&& optional) noexcept
	{
		isValue_ = optional.isValue_;
		if (isValue_)
		{
			value_ = bzd::move(optional.value_);
		}
		return *this;
	}
	//~OptionalTrivialStorage() noexcept = default;

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
	constexpr Optional(Self&& optional) noexcept : storage_{bzd::move(optional.storage_)} {}
	constexpr Self& operator=(Self&& optional) noexcept = default;

	// Support for bzd::nullopt
	constexpr Optional(const OptionalNull&) noexcept : Optional{} {}

	// Forward constructor to storage type for all non-self typed
	template <class U, typename = typeTraits::EnableIf<!IsSelf<U>::value>>
	constexpr Optional(U&& value) : storage_{bzd::forward<U>(value), true}
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

	// Constructs the contained value in-place.
	// If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
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

// \brief Type managing an optional contained value, i.e. a value that may or
// may not be present.
template <class T>
using Optional = impl::Optional<T>;
} // namespace bzd

/*
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
	using ValueContainer = T; //bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

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
	constexpr bool hasValue() const noexcept { return isValue_; }
	constexpr explicit operator bool() const noexcept { return hasValue(); }

	constexpr const Value& valueOr(const Value& defaultValue) const noexcept
	{
		return (isValue_) ? data_.template get<ValueContainer>() : defaultValue;
	}

	constexpr const Value& value() const
	{
		bzd::assert::isTrue(isValue_);
		return data_.template get<ValueContainer>();
	}

	constexpr Value& valueMutable()
	{
		bzd::assert::isTrue(isValue_);
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
		isValue_ = true;
	}

	constexpr void reset() noexcept
	{
		if (isValue_)
		{
			data_ = OptionalNull::make();
			isValue_ = false;
		}
	}

private:
	bzd::BoolType isValue_{false};
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
*/