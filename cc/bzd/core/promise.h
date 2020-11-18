#pragma once

#include "bzd/container/result.h"
#include "bzd/container/optional.h"

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
public:
	using ResultType = bzd::Result<V, E>;
	using OptionalResultType = bzd::Optional<bzd::Result<V, E>>;

public:
	constexpr Promise() noexcept : interface::Promise{} {}
	constexpr bool isReady() const noexcept { return static_cast<bool>(result_); }
	constexpr void setResult(OptionalResultType&& optionalResult) noexcept { result_ = bzd::move(optionalResult); }
	constexpr void setResult(ResultType&& result) noexcept { result_ = bzd::move(result); }

	constexpr ResultType&& getResult() noexcept
	{
		bzd::assert::isTrue(result_);
		return bzd::move(*result_);
	}

protected:
	OptionalResultType result_{};
};
} // namespace bzd
