#pragma once

#include "cc/bzd/core/async/awaitables/default.hh"
#include "cc/bzd/meta/always_false.hh"

namespace bzd::coroutine::impl {

template <class Async, Size index>
class Propagate : public Awaiter<Async>
{
private:
	using PromiseType = typename Async::PromiseType;

public:
	using Awaiter<Async>::Awaiter;
	constexpr auto await_resume() noexcept
	{
		if constexpr (PromiseType::resultTypeIsResult)
		{
			auto result{this->async_.moveResultOut()};
			bzd::assert::isTrue(result.hasValue());
			return bzd::move(result.valueMutable());
		}
		else if constexpr (PromiseType::resultTypeIsTupleOfOptionalResultsWithError)
		{
			auto tuple{this->async_.moveResultOut()};
			auto& maybeResult{tuple.template get<index>()};
			bzd::assert::isTrue(maybeResult.hasValue());
			bzd::assert::isTrue(maybeResult.value().hasValue());
			return bzd::move(maybeResult.valueMutable().valueMutable());
		}
		else
		{
			static_assert(bzd::meta::alwaysFalse<Async>, "This type of Async cannot be used with assert(...).");
		}
	}
};

} // namespace bzd::coroutine::impl
