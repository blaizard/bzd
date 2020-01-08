#pragma once

#include "bzd/core/assert.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"
#include "bzd/utility/reference_wrapper.h"

namespace bzd {
namespace impl {
template <class T>
class Optional
{
private:
	using Value = typename bzd::typeTraits::removeReference<T>::type;
	using ValueContainer = typename bzd::typeTraits::conditional<bzd::typeTraits::isReference<T>::value, bzd::ReferenceWrapper<T>, T>::type;

public:
	constexpr Optional(T &&value) : isValue_(true), value_(bzd::forward<T>(value)) {}

	constexpr Optional() : isValue_(false), empty_{} {}

	constexpr operator bool() const noexcept { return isValue_; }

	constexpr const Value &valueOr(const Value &defaultValue) const { return (isValue_) ? value_ : defaultValue; }

	constexpr const Value &operator*() const
	{
		bzd::assert::isTrue(isValue_);
		return value_;
	}

	constexpr Value &operator*()
	{
		bzd::assert::isTrue(isValue_);
		return value_;
	}

	constexpr const Value *operator->() const
	{
		bzd::assert::isTrue(isValue_);
		return &value_;
	}

	constexpr Value *operator->()
	{
		bzd::assert::isTrue(isValue_);
		return &value_;
	}

protected:
	const bool isValue_;
	union {
		UInt8Type empty_;
		ValueContainer value_;
	};
};
} // namespace impl

/**
 * \brief Type managing an optional contained value, i.e. a value that may or
 * may not be present.
 */
template <class T>
using Optional = impl::Optional<T>;
} // namespace bzd
