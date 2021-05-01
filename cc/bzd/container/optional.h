#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/type_traits/is_trivially_destructible.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

#include <new> // Used only for placement new

namespace bzd::impl {

// nullopt

struct OptionalNull
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
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	constexpr OptionalNonTrivialStorage() : isValue_{false}, empty_{} {}

	template <class U>
	constexpr OptionalNonTrivialStorage(U&& value) : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	~OptionalNonTrivialStorage()
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
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	constexpr OptionalTrivialStorage() : isValue_{false}, empty_{} {}

	template <class U>
	constexpr OptionalTrivialStorage(U&& value) : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

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
	using Value = bzd::typeTraits::RemoveReference<T>;
	using StorageType =
		bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<T>, OptionalTrivialStorage<T>, OptionalNonTrivialStorage<T>>;
	using ValueContainer = typename StorageType::ValueContainer;

public: // Constructors
	template <class U>
	constexpr Optional(U&& value) : storage_{bzd::forward<U>(value)}
	{
	}

	constexpr Optional(const OptionalNull& null) : Optional{} {}

	explicit constexpr Optional() : storage_{} {}

public: // Move
	constexpr Optional(Optional<T>&& optional) { *this = bzd::move(optional); }

	// Move assignment
	constexpr void operator=(Optional<T>&& optional)
	{
		storage_.isValue_ = optional.storage_.isValue_;
		if (storage_.isValue_) storage_.value_ = bzd::move(optional.storage_.value_);
	}

public:
	constexpr operator bool() const noexcept { return storage_.isValue_; }

	constexpr const Value& valueOr(const Value& defaultValue) const { return (storage_.isValue_) ? storage_.value_ : defaultValue; }

	constexpr const Value& operator*() const
	{
		bzd::assert::isTrue(storage_.isValue_);
		return storage_.value_;
	}

	constexpr Value& operator*()
	{
		bzd::assert::isTrue(storage_.isValue_);
		return storage_.value_;
	}

	constexpr const Value* operator->() const
	{
		bzd::assert::isTrue(storage_.isValue_);
		return &storage_.value_;
	}

	constexpr Value* operator->()
	{
		bzd::assert::isTrue(storage_.isValue_);
		return &storage_.value_;
	}

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
