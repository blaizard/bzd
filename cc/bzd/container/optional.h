#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_trivially_destructible.h"


namespace bzd::impl {

struct OptionalNull
{
public:
	static constexpr OptionalNull make() { return OptionalNull{}; }

private:
	explicit constexpr OptionalNull() noexcept {}
};

template <class T>
class OptionalNonTrivialDestructorStorage
{
protected:
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

public: // Constructors
	template <class U>
	constexpr OptionalNonTrivialDestructorStorage(U&& value) : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr OptionalNonTrivialDestructorStorage(const OptionalNull& null) : OptionalNonTrivialDestructorStorage{} {}

	explicit constexpr OptionalNonTrivialDestructorStorage() : isValue_{false}, empty_{} {}

public:
	~OptionalNonTrivialDestructorStorage()
	{
		if (isValue_) value_.~ValueContainer();
	}

protected:
	bool isValue_;
	union {
		bzd::UInt8Type empty_;
		ValueContainer value_;
	};
};

template <class T>
class OptionalTrivialDestructorStorage
{
protected:
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

public: // Constructors
	template <class U>
	constexpr OptionalTrivialDestructorStorage(U&& value) : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr OptionalTrivialDestructorStorage(const OptionalNull& null) : OptionalTrivialDestructorStorage{} {}

	explicit constexpr OptionalTrivialDestructorStorage() : isValue_{false}, empty_{} {}

protected:
	bool isValue_;
	union {
		bzd::UInt8Type empty_;
		ValueContainer value_;
	};
};

template <class T>
class Optional : public bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<T>, OptionalTrivialDestructorStorage<T>, OptionalNonTrivialDestructorStorage<T>>
{
protected:
	using Value = bzd::typeTraits::RemoveReference<T>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;
	using Parent = bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<T>, OptionalTrivialDestructorStorage<T>, OptionalNonTrivialDestructorStorage<T>>;

public: // Constructors
	template <class... Args>
	constexpr Optional(Args&&... value) : Parent{bzd::forward<Args>(value)...}
	{
	}

public: // Move
	constexpr Optional(Optional<T>&& optional) { *this = bzd::move(optional); }

	// Move assignment
	constexpr void operator=(Optional<T>&& optional)
	{
		this->isValue_ = optional.isValue_;
		if (this->isValue_) this->value_ = bzd::move(optional.value_);
	}

public:
	constexpr operator bool() const noexcept { return this->isValue_; }

	constexpr const Value& valueOr(const Value& defaultValue) const { return (this->isValue_) ? this->value_ : defaultValue; }

	constexpr const Value& operator*() const
	{
		bzd::assert::isTrue(this->isValue_);
		return this->value_;
	}

	constexpr Value& operator*()
	{
		bzd::assert::isTrue(this->isValue_);
		return this->value_;
	}

	constexpr const Value* operator->() const
	{
		bzd::assert::isTrue(this->isValue_);
		return &this->value_;
	}

	constexpr Value* operator->()
	{
		bzd::assert::isTrue(this->isValue_);
		return &this->value_;
	}
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

