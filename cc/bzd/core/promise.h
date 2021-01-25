#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/type_traits/invoke_result.h"

namespace bzd::interface {
class Promise
{
protected:
	constexpr Promise() noexcept = default;

public:
	virtual bool poll() = 0;
	virtual ~Promise() = default;
};
} // namespace bzd::interface

namespace bzd::impl {
template <class V, class E>
class Promise : public bzd::interface::Promise
{
public: // Types.
	using ResultType = bzd::Result<V, E>;
	using ReturnType = bzd::Optional<bzd::Result<V, E>>;

public: // Constructors.
	constexpr Promise() noexcept : interface::Promise{} {}

public:
	constexpr bool isReady() const noexcept { return static_cast<bool>(return_); }
	constexpr void setResult(ReturnType&& optionalResult) noexcept { return_ = bzd::move(optionalResult); }
	constexpr void setResult(ResultType&& result) noexcept { return_ = bzd::move(result); }

	constexpr ResultType&& getResult() noexcept
	{
		bzd::assert::isTrue(return_);
		return bzd::move(*return_);
	}

protected:
	ReturnType return_{};
};
} // namespace bzd::impl

namespace bzd {
	/*
template <class V, class E>
class Promise : public bzd::impl::Promise<V, E>
{
private:
	using FctPtrType = bzd::FctPtrType;

public:
	template <class Object, class T>
	Promise() : bzd::impl::Promise<V, E>{}
	{
	}

	bool poll() override
	{
		//setResult(poll_());
		return isReady();
	}

public:
	// Promise type
	enum class Type
	{
		POLLING,
		EVENT
	};
	Type type_;
	//bzd::Variant<>
	// Need to abstract the member->fct call. See functionoid for a possible implementation.
	// https://isocpp.org/wiki/faq/pointers-to-members#typedef-for-ptr-to-memfn
};
*/
template <class V, class E, class PollFct>
class PromisePoll : public bzd::impl::Promise<V, E>
{
private:
	using bzd::impl::Promise<V, E>::setResult;

public:
	using bzd::impl::Promise<V, E>::isReady;

	template <class T = PollFct>
	constexpr PromisePoll(T&& callack) : bzd::impl::Promise<V, E>{}, poll_{bzd::forward<T>(callack)}
	{
	}

	bool poll() override
	{
		setResult(poll_());
		return isReady();
	}

private:
	// Callback is statefull, therefore it cannot be const.
	PollFct poll_;
};

template <class V = void, class E = bzd::BoolType>
class PromiseReturnType : public bzd::Optional<bzd::Result<V, E>>
{
public:
	using Value = V;
	using Error = E;
	using bzd::Optional<bzd::Result<V, E>>::Optional;
};

template <class T, class V = typename bzd::typeTraits::InvokeResult<T>::Value, class E = typename bzd::typeTraits::InvokeResult<T>::Error>
constexpr auto makePromise(T&& callback)
{
	return bzd::PromisePoll<V, E, T>(bzd::forward<T>(callback));
}
} // namespace bzd
