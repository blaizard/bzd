#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

namespace bzd::impl {

struct OptionalNull
{
public:
	static constexpr OptionalNull make() { return OptionalNull{}; }

private:
	explicit constexpr OptionalNull() noexcept {}
};

template <class T>
class Optional
{
private:
	using Value = bzd::typeTraits::RemoveReference<T>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

public: // Constructors
	template <class U>
	constexpr Optional(U&& value) : isValue_{true}, value_{bzd::forward<U>(value)}
	{
	}

	constexpr Optional(const OptionalNull& null) : Optional{} {}

	explicit constexpr Optional() : isValue_{false}, empty_{} {}

public: // Move
	constexpr Optional(Optional<T>&& optional) { *this = bzd::move(optional); }

	// Move assignment
	constexpr void operator=(Optional<T>&& optional)
	{
		isValue_ = optional.isValue_;
		if (isValue_) value_ = bzd::move(optional.value_);
	}

public:
	constexpr operator bool() const noexcept { return isValue_; }

	constexpr const Value& valueOr(const Value& defaultValue) const { return (isValue_) ? value_ : defaultValue; }

	constexpr const Value& operator*() const
	{
		bzd::assert::isTrue(isValue_);
		return value_;
	}

	constexpr Value& operator*()
	{
		bzd::assert::isTrue(isValue_);
		return value_;
	}

	constexpr const Value* operator->() const
	{
		bzd::assert::isTrue(isValue_);
		return &value_;
	}

	constexpr Value* operator->()
	{
		bzd::assert::isTrue(isValue_);
		return &value_;
	}

	~Optional()
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
