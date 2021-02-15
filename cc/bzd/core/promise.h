#pragma once

#include "bzd/container/function_view.h"
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
	virtual ~Promise() = default;

public:
	virtual bool poll() = 0;

	/**
	 * Set the promise and its associated task as pending and attach it to a list.
	 */
	void setPending(bzd::NonOwningList<bzd::interface::Promise>& list);

	/**
	 * Set back the promise and its associated task as active and detach it from the list.
	 */
	void setActive(bzd::NonOwningList<bzd::interface::Promise>& list);

protected:
	friend class bzd::interface::Task;

	bzd::interface::Task* task_{nullptr};
};
} // namespace bzd::interface

namespace bzd {
template <class V = void, class E = bzd::BoolType>
class Promise : public bzd::interface::Promise
{
public: // Types.
	using ResultType = bzd::Result<V, E>;
	using ReturnType = bzd::Optional<bzd::Result<V, E>>;

public: // Constructors.
	template <class... Args>
	constexpr Promise(Args&&... args) noexcept : interface::Promise{}, poll_{bzd::forward<Args>(args)...}
	{
	}

public:
	constexpr bool isReady() const noexcept { return static_cast<bool>(return_); }
	constexpr void setResult(ReturnType&& optionalResult) noexcept { return_ = bzd::move(optionalResult); }
	constexpr void setResult(ResultType&& result) noexcept { return_ = bzd::move(result); }

	constexpr ResultType&& getResult() noexcept
	{
		bzd::assert::isTrue(return_);
		return bzd::move(*return_);
	}

	bool poll() override final
	{
		if (!isReady())
		{
			setResult(poll_(*this));
		}
		return isReady();
	}

	// When lifespan of this promise terminates, remove it from wherever it was.
	~Promise() { bzd::ignore = this->pop(); }

protected:
	ReturnType return_{};
	bzd::FunctionView<ReturnType(bzd::interface::Promise&)> poll_{};
};

template <class V, class E, class PollFct>
class PromisePoll : public bzd::Promise<V, E>
{
private:
	using ReturnType = bzd::Optional<bzd::Result<V, E>>;

public:
	using bzd::Promise<V, E>::isReady;

	template <class T = PollFct>
	constexpr PromisePoll(T&& callack) : bzd::Promise<V, E>{poll_}, poll_{bzd::forward<T>(callack)}
	{
	}

private:
	// Callback is statefull, therefore it cannot be const.
	PollFct poll_;
};

template <class T,
		  class V = typename bzd::typeTraits::InvokeResult<T, bzd::interface::Promise&>::Value::Value,
		  class E = typename bzd::typeTraits::InvokeResult<T, bzd::interface::Promise&>::Value::Error>
constexpr auto makePromise(T&& callback)
{
	return bzd::PromisePoll<V, E, T>(bzd::forward<T>(callback));
}
} // namespace bzd
