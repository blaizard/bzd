#pragma once

#include "bzd/core/assert.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/utility/decay.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"
#include "bzd/utility/reference_wrapper.h"

namespace bzd {
namespace impl {
// Forward declaration for the "friend" attribute
template <class T, class E>
class Expected;

/**
 * Internal class used to create an unexpected object type.
 */
template <class E>
class Unexpected
{
public:
	constexpr Unexpected(const E& error) : error_(error) {}

private:
	template <class A, class B>
	friend class bzd::impl::Expected;
	E error_;
};

template <class T, class E>
class Expected
{
private:
	using Value = typename bzd::typeTraits::removeReference<T>::type;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>::value, bzd::ReferenceWrapper<T>, T>;

public:
	constexpr Expected(T&& value) : isError_(false), value_(bzd::forward<T>(value)) {}

	template <class U>
	constexpr Expected(impl::Unexpected<U>&& u) : isError_(true), error_(u.error_)
	{
	}

	// Move constructor
	constexpr Expected(Expected<T, E>&& e) : isError_(e.isError_)
	{
		if (isError_)
			error_ = bzd::move(e.error_);
		else
			value_ = bzd::move(e.value_);
	}

	~Expected()
	{
		if (isError_)
			error_.~E();
		else
			value_.~ValueContainer();
	}

	constexpr operator bool() const noexcept { return !isError_; }

	constexpr const E& error() const
	{
		bzd::assert::isTrue(isError_);
		return error_;
	}

	constexpr const Value& operator*() const
	{
		bzd::assert::isTrue(!isError_);
		return value_;
	}

	constexpr Value& operator*()
	{
		bzd::assert::isTrue(!isError_);
		return value_;
	}

	constexpr const Value* operator->() const
	{
		bzd::assert::isTrue(!isError_);
		return &value_;
	}

	constexpr Value* operator->()
	{
		bzd::assert::isTrue(!isError_);
		return &value_;
	}

protected:
	const bool isError_;
	union {
		ValueContainer value_;
		E error_;
	};
};

template <class E>
class Expected<void, E> : private Expected<void*, E>
{
public:
	using Expected<void*, E>::Expected;
	using Expected<void*, E>::operator bool;
	using Expected<void*, E>::error;

	constexpr Expected() : Expected<void*, E>(nullptr) {}
};
} // namespace impl

/**
 * \brief This is the type used for returning and propagating errors.
 *
 * It is a variants with 2 states, valid, representing success and containing a
 * value, and error, representing error and containing an error value.
 */
template <class T, class E>
using Expected = impl::Expected<T, E>;

template <class E>
constexpr impl::Unexpected<typename bzd::decay<E>::type> makeUnexpected(E&& e)
{
	return impl::Unexpected<typename bzd::decay<E>::type>(bzd::forward<E>(e));
}
} // namespace bzd
