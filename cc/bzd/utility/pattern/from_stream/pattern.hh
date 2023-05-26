#pragma once

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/utility/pattern/async.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/regexp/async.hh"

namespace bzd {

template <concepts::constexprStringView T>
struct FromStream<T>
{
public:
	template <concepts::readerAsync Input, class... Args>
	static bzd::Async<Size> process(Input&& input, const T& pattern, Args&&... args) noexcept
	{
		const auto [context, processor] = bzd::pattern::impl::makeAsync<Input&, Schema>(pattern, args...);
		Size counter{0u};

		// Run-time call
		for (const auto& fragment : context)
		{
			if (!fragment.str.empty())
			{
				bzd::RegexpAsync regexp{fragment.str};
				const auto size = co_await !regexp.match(input);
				counter += size;
			}
			if (fragment.isMetadata)
			{
				const auto size = co_await !processor.process(input, fragment);
				counter += size;
			}
		}

		co_return counter;
	}

private:
	class Schema
	{
	public:
		/// Get the specialization associated with a type.
		template <class Value>
		using Specialization = typename typeTraits::template FromStream<Value>;
	};
};

} // namespace bzd
