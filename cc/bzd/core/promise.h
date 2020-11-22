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

namespace bzd {
template <class V, class E>
class Promise : public bzd::interface::Promise
{
public: // Types.
	using ResultType = bzd::Result<V, E>;
	class ReturnType : public bzd::Optional<bzd::Result<V, E>>
	{
	public:
		using Value = V;
		using Error = E;
		using bzd::Optional<bzd::Result<V, E>>::Optional;
	};

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

} // namespace bzd

namespace bzd::impl
{
template <class V, class E, class PollFct>
class PromisePoll : public bzd::Promise<V, E>
{
private:
	using bzd::Promise<V, E>::setResult;

public:
	using bzd::Promise<V, E>::isReady;

	template <class T = PollFct>
	PromisePoll(T&& callack) : bzd::Promise<V, E>{}, poll_{bzd::forward<T>(callack)}
	{
	}
	bool poll() override
	{
		setResult(poll_());
		return isReady();
	}

private:
	const PollFct poll_;
};
}

namespace bzd
{
template <class T, class V = typename bzd::typeTraits::InvokeResult<T>::Value, class E = typename bzd::typeTraits::InvokeResult<T>::Error>
auto makePromise(T&& callback)
{
	return bzd::impl::PromisePoll<V, E, T>(bzd::forward<T>(callback));
}
}
