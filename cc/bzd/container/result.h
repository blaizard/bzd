#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/type_traits/is_trivially_destructible.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

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
class ResultNonTrivialStorage;
template <class T, class E>
class ResultTrivialStorage;

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
	friend class bzd::impl::ResultNonTrivialStorage;
	template <class A, class B>
	friend class bzd::impl::ResultTrivialStorage;
	E error_;
};

// Storage

template <class T, class E>
struct ResultNonTrivialStorage
{
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	template <class U>
	constexpr ResultNonTrivialStorage(U&& value) : isError_(false), value_(bzd::forward<U>(value))
	{
	}

	template <class U>
	constexpr ResultNonTrivialStorage(impl::Error<U>&& u) : isError_(true), error_(u.error_)
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
	constexpr ResultTrivialStorage(U&& value) : isError_(false), value_(bzd::forward<U>(value))
	{
	}

	template <class U>
	constexpr ResultTrivialStorage(impl::Error<U>&& u) : isError_(true), error_(u.error_)
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
private:
	using Value = bzd::typeTraits::RemoveReference<T>;
	using StorageType =
		bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<T> && bzd::typeTraits::isTriviallyDestructible<E>,
									 ResultTrivialStorage<T, E>,
									 ResultNonTrivialStorage<T, E>>;

public:
	template <class U>
	constexpr Result(U&& value) : storage_{bzd::forward<U>(value)}
	{
	}

	// Move constructor
	constexpr Result(Result<T, E>&& result) { *this = bzd::move(result); }

	// Move assignment
	constexpr void operator=(Result<T, E>&& result)
	{
		storage_.isError_ = result.storage_.isError_;
		if (storage_.isError_)
			storage_.error_ = bzd::move(result.storage_.error_);
		else
			storage_.value_ = bzd::move(result.storage_.value_);
	}

	constexpr operator bool() const noexcept { return !storage_.isError_; }

	constexpr const E& error() const
	{
		bzd::assert::isTrue(storage_.isError_);
		return storage_.error_;
	}

	constexpr const Value& operator*() const
	{
		bzd::assert::isTrue(!storage_.isError_);
		return storage_.value_;
	}

	constexpr Value& operator*()
	{
		bzd::assert::isTrue(!storage_.isError_);
		return storage_.value_;
	}

	constexpr const Value* operator->() const
	{
		bzd::assert::isTrue(!storage_.isError_);
		return &storage_.value_;
	}

	constexpr Value* operator->()
	{
		bzd::assert::isTrue(!storage_.isError_);
		return &storage_.value_;
	}

protected:
	StorageType storage_;
};

template <class E>
class Result<void, E> : private Result<void*, E>
{
public:
	using Result<void*, E>::Result;
	using Result<void*, E>::operator bool;
	using Result<void*, E>::error;

	// constexpr Result() : Result<void*, E>(nullptr) {}
	constexpr Result(const ResultNull&) : Result<void*, E>(nullptr) {}
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

template <class E>
constexpr impl::Error<bzd::typeTraits::Decay<E>> makeError(E&& e)
{
	return impl::Error<bzd::typeTraits::Decay<E>>(bzd::forward<E>(e));
}

} // namespace bzd
