#pragma once

#include "bzd/container/reference_wrapper.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_constructible.h"
#include "bzd/type_traits/is_reference.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/is_trivially_destructible.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

namespace bzd::impl {

class ResultNull
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
	using Self = ResultNonTrivialStorage<T, E>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	template <class U>
	constexpr ResultNonTrivialStorage(U&& value) noexcept : isError_{false}, value_{bzd::forward<U>(value)}
	{
	}

	template <class U>
	constexpr ResultNonTrivialStorage(U&& value, bool) noexcept : isError_{true}, error_{bzd::forward<U>(value)}
	{
	}

	constexpr ResultNonTrivialStorage(const Self& value) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept = default;
	constexpr ResultNonTrivialStorage(Self&& value) noexcept = default;
	constexpr Self& operator=(Self&& optional) noexcept = default;

	~ResultNonTrivialStorage() noexcept
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
	using Self = ResultTrivialStorage<T, E>;
	using ValueContainer = bzd::typeTraits::Conditional<bzd::typeTraits::isReference<T>, bzd::ReferenceWrapper<T>, T>;

	template <class U>
	constexpr ResultTrivialStorage(U&& value) noexcept : isError_{false}, value_{bzd::forward<U>(value)}
	{
	}

	template <class U>
	constexpr ResultTrivialStorage(U&& value, bool) noexcept : isError_{true}, error_{bzd::forward<U>(value)}
	{
	}

	constexpr ResultTrivialStorage(const Self& value) noexcept = default;
	constexpr Self& operator=(const Self& optional) noexcept = default;
	constexpr ResultTrivialStorage(Self&& value) noexcept = default;
	constexpr Self& operator=(Self&& optional) noexcept = default;
	~ResultTrivialStorage() noexcept = default;

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
	using Self = Result<T, E>;
	using NonVoidValue = bzd::typeTraits::Conditional<bzd::typeTraits::isSame<Value, void>, void*, Value>;
	using StorageType = bzd::typeTraits::Conditional<bzd::typeTraits::isTriviallyDestructible<NonVoidValue> &&
														 bzd::typeTraits::isTriviallyDestructible<Error>,
													 ResultTrivialStorage<NonVoidValue, Error>,
													 ResultNonTrivialStorage<NonVoidValue, Error>>;

public: // Constructors
	constexpr Result(const ResultNull&) noexcept : storage_{nullptr} {}

	constexpr Result(const NonVoidValue& value) noexcept : storage_{value} {}

	template <class U>
	constexpr Result(impl::Error<U>&& u) noexcept : storage_{bzd::move(u.error_), false}
	{
	}

	// Copy/move constructore/assignment.
	constexpr Result(const Self& result) noexcept = default;
	constexpr Self& operator=(const Self& result) noexcept = default;
	constexpr Result(Self&& result) noexcept = default;
	constexpr Self& operator=(Self&& result) noexcept = default;

public: // API
	constexpr bool hasValue() const noexcept { return !storage_.isError_; }
	constexpr bool hasError() const noexcept { return storage_.isError_; }

	constexpr explicit operator bool() const noexcept { return hasValue(); }

	constexpr const E& error() const
	{
		bzd::assert::isTrue(storage_.isError_);
		return storage_.error_;
	}

	constexpr const bzd::typeTraits::RemoveReference<NonVoidValue>& value() const
	{
		bzd::assert::isTrue(!storage_.isError_);
		return storage_.value_;
	}

	constexpr bzd::typeTraits::RemoveReference<NonVoidValue>& valueMutable()
	{
		bzd::assert::isTrue(!storage_.isError_);
		return storage_.value_;
	}

	constexpr const bzd::typeTraits::RemoveReference<NonVoidValue>* operator->() const { return &value(); }

	constexpr bzd::typeTraits::RemoveReference<NonVoidValue>* operator->() { return &valueMutable(); }

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
