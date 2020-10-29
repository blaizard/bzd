#pragma once

#include "bzd/core/assert.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"
#include "bzd/utility/reference_wrapper.h"

namespace bzd::impl {
// Forward declaration for the "friend" attribute
template <class T, class E>
class Result;

/**
 * Internal class used to create an unexpected result object type.
 */
template <class E>
class Error
{
public:
	constexpr Error(const E& error) : error_(error) {}

private:
	template <class A, class B>
	friend class bzd::impl::Result;
	E error_;
};

template <class T, class E>
class Result
{
private:
	using Value = bzd::typeTraits::RemoveReference<T>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

public:
	constexpr Result(T&& value) : isError_(false), value_(bzd::forward<T>(value)) {}

	template <class U>
	constexpr Result(impl::Error<U>&& u) : isError_(true), error_(u.error_)
	{
	}

	// Move constructor
	constexpr Result(Result<T, E>&& e) : isError_(e.isError_)
	{
		if (isError_)
			error_ = bzd::move(e.error_);
		else
			value_ = bzd::move(e.value_);
	}

	~Result()
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
class Result<void, E> : private Result<void*, E>
{
public:
	using Result<void*, E>::Result;
	using Result<void*, E>::operator bool;
	using Result<void*, E>::error;

	constexpr Result() : Result<void*, E>(nullptr) {}
};
} // namespace bzd::impl

namespace bzd {
/**
 * \brief This is the type used for returning and propagating errors.
 *
 * It is a variants with 2 states, valid, representing success and containing a
 * value, and error, representing error and containing an error value.
 */
template <class T, class E = bool>
using Result = impl::Result<T, E>;

template <class E>
constexpr impl::Error<bzd::typeTraits::Decay<E>> makeError(E&& e)
{
	return impl::Error<bzd::typeTraits::Decay<E>>(bzd::forward<E>(e));
}

} // namespace bzd
