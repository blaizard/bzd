#pragma once

#include "bzd/container/impl/non_owning_list.h"
#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/type_traits/invoke_result.h"
#include "bzd/utility/ignore.h"

namespace bzd::interface {
class Task;

class Promise : public bzd::NonOwningListElement</*MultiContainer*/ true>
{
protected:
	constexpr Promise() noexcept = default;

public:
	virtual bool poll() = 0;
	virtual ~Promise() = default;

	// virtual void detach() {}

	bzd::interface::Task* task_{nullptr};
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
	constexpr Promise(const BoolType isEvent) noexcept : interface::Promise{}, isEvent_{isEvent} {}

public:
	constexpr bool isReady() const noexcept { return static_cast<bool>(return_); }
	constexpr bool isEvent() const noexcept { return isEvent_; }
	constexpr void setResult(ReturnType&& optionalResult) noexcept { return_ = bzd::move(optionalResult); }
	constexpr void setResult(ResultType&& result) noexcept { return_ = bzd::move(result); }

	constexpr ResultType&& getResult() noexcept
	{
		bzd::assert::isTrue(return_);
		return bzd::move(*return_);
	}

	// When lifespan of this promise terminates, remove it from wherever it was.
	~Promise() { bzd::ignore = this->pop(); }

protected:
	BoolType isEvent_;
	ReturnType return_{};
};
} // namespace bzd::impl

namespace bzd {

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
		// setResult(poll_());
		return this->isReady();
	}

public:
	// Promise type
	enum class Type
	{
		POLLING,
		EVENT
	};
	Type type_;
	// bzd::Variant<>
	// Need to abstract the member->fct call. See functionoid for a possible implementation.
	// https://isocpp.org/wiki/faq/pointers-to-members#typedef-for-ptr-to-memfn
};

template <class V, class E, class PollFct>
class PromisePoll : public bzd::impl::Promise<V, E>
{
private:
	using bzd::impl::Promise<V, E>::setResult;

public:
	using bzd::impl::Promise<V, E>::isReady;

	template <class T = PollFct>
	constexpr PromisePoll(T&& callack, const BoolType isEvent) : bzd::impl::Promise<V, E>{isEvent}, poll_{bzd::forward<T>(callack)}
	{
	}

	bool poll() override
	{
		if (!isReady())
		{
			setResult(poll_(*this));
		}
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

template <class T,
		  class V = typename bzd::typeTraits::InvokeResult<T, bzd::interface::Promise&>::Value,
		  class E = typename bzd::typeTraits::InvokeResult<T, bzd::interface::Promise&>::Error>
constexpr auto makePromise(T&& callback, const BoolType isEvent = false)
{
	return bzd::PromisePoll<V, E, T>(bzd::forward<T>(callback), isEvent);
}
} // namespace bzd
