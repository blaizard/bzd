#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/is_trivially_destructible.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"
#include "bzd/type_traits/enable_if.h"

namespace bzd::impl {

struct ResultNull
{
public:
	static constexpr ResultNull make() { return ResultNull{}; }

private:
	explicit constexpr ResultNull() noexcept {}
};

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

// Storage

template <class T, class E>
struct ResultNonTrivialStorage
{
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	template <class U>
	constexpr ResultNonTrivialStorage(U&& value) : isError_{false}, value_{bzd::forward<U>(value)}
	{
	}

	template <class U>
	constexpr ResultNonTrivialStorage(U&& value, bool) : isError_{true}, error_{bzd::forward<U>(value)}
	{
	}

	~ResultNonTrivialStorage()
	{
		if (isError_)
			error_.~E();
		else
			value_.~ValueContainer();
	}

	bool isError_;
	union {
		ValueContainer value_;
		E error_;
	};
};

template <class T, class E>
struct ResultTrivialStorage
{
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	template <class U>
	constexpr ResultTrivialStorage(U&& value) : isError_{false}, value_{bzd::forward<U>(value)}
	{
	}

	template <class U>
	constexpr ResultTrivialStorage(U&& value, bool) : isError_{true}, error_{bzd::forward<U>(value)}
	{
	}

	bool isError_;
	union {
		ValueContainer value_;
		E error_;
	};
};

template <class T, class E>
class Result
{
public:
	using Value = T;
	using Error = E;

private:
	using NonVoidValue = bzd::typeTraits::Conditional<bzd::typeTraits::isSame<Value, void>, void*, Value>;
	using StorageType = bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<NonVoidValue> &&
														 bzd::typeTraits::isTriviallyDestructible<Error>,
													 ResultTrivialStorage<NonVoidValue, Error>,
													 ResultNonTrivialStorage<NonVoidValue, Error>>;

public:
	constexpr Result(const ResultNull&) noexcept : storage_{nullptr} {}

	// Ensure perfect forwarding is on
	template <class U, class V = bzd::typeTraits::EnableIf<bzd::typeTraits::isSame<U, NonVoidValue>>>
	constexpr Result(U&& value) noexcept : storage_{bzd::forward<U>(value)} {}

	template <class U>
	constexpr Result(impl::Error<U>&& u) noexcept : storage_{bzd::move(u.error_), false} {}

	// Copy constructore/assignment.
	constexpr Result(const impl::Result<T, E>& result) noexcept : storage_{result.storage_} {}
	constexpr void operator=(const impl::Result<T, E>& result) noexcept { storage_ = result.storage_; }

	// Move constructor/assignment.
	constexpr Result(impl::Result<T, E>&& result) noexcept : storage_{bzd::move(result.storage_)} {}
	constexpr void operator=(impl::Result<T, E>&& result) noexcept { storage_ = bzd::move(result.storage_); }

	constexpr operator bool() const noexcept { return !storage_.isError_; }

	constexpr const E& error() const
	{
		bzd::assert::isTrue(storage_.isError_);
		return storage_.error_;
	}

	constexpr const bzd::typeTraits::RemoveReference<NonVoidValue>& operator*() const
	{
		bzd::assert::isTrue(!storage_.isError_);
		return storage_.value_;
	}

	constexpr bzd::typeTraits::RemoveReference<NonVoidValue>& operator*()
	{
		bzd::assert::isTrue(!storage_.isError_);
		return storage_.value_;
	}

	constexpr const bzd::typeTraits::RemoveReference<NonVoidValue>* operator->() const
	{
		bzd::assert::isTrue(!storage_.isError_);
		return &storage_.value_;
	}

	constexpr bzd::typeTraits::RemoveReference<NonVoidValue>* operator->()
	{
		bzd::assert::isTrue(!storage_.isError_);
		return &storage_.value_;
	}

protected:
	template <class A, class B>
	friend class bzd::impl::Result;
	StorageType storage_;
};

} // namespace bzd::impl

namespace bzd {
/**
 * \brief This is the type used for returning and propagating errors.
 *
 * It is a variants with 2 states, valid, representing success and containing a
 * value, and error, representing error and containing an error value.
 */
template <class T = void, class E = bzd::BoolType>
using Result = impl::Result<T, E>;

constexpr impl::ResultNull nullresult = impl::ResultNull::make();

template <class E = bzd::BoolType>
constexpr impl::Error<bzd::typeTraits::Decay<E>> makeError(E&& e = true)
{
	return impl::Error<bzd::typeTraits::Decay<E>>(bzd::forward<E>(e));
}

} // namespace bzd
