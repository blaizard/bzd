#pragma once

#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/pattern/from_string/integral.hh"

namespace bzd {

template <concepts::asyncInputByteCopyableRange Input, class Callback>
bzd::Async<Size> forEach(Input&& input, Callback&& callback) noexcept
{
	auto it = bzd::begin(input);
	auto end = bzd::end(input);
	Size count{0u};
	while (true)
	{
		if (it == end)
		{
			auto mayebSuccess = co_await input.next();
			if (mayebSuccess)
			{
				it = bzd::begin(input);
				end = bzd::end(input);
			}
			else if (mayebSuccess.error().getType() != bzd::ErrorType::eof)
			{
				co_return bzd::move(mayebSuccess).propagate();
			}
			// If there are still no input after fetching new data.
			if (it == end)
			{
				break;
			}
		}

		if (!callback(*it))
		{
			break;
		}
		++count;
		++it;
	}

	co_return count;
}

template <concepts::integral T>
struct FromStream<T> : public FromString<T>
{
	using Metadata = typename bzd::FromString<T>::Metadata;

	template <concepts::asyncInputByteCopyableRange Input>
	static bzd::Async<Size> process(Input&& input, T& output, const Metadata metadata = Metadata{}) noexcept
	{
		(void)metadata;
		output = 0u;
		const auto count = co_await !forEach(bzd::forward<Input>(input), [&](const auto input) -> Bool {
			const char c = static_cast<char>(input);
			if (c < '0' || c > '9')
			{
				return false;
			}
			output = output * 10 + (c - '0');
			return true;
		});
		if (!count)
		{
			co_return bzd::error::Failure{"No integral"};
		}
		co_return count;
	}
};

} // namespace bzd
