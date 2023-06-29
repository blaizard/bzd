#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/utility/regexp/regexp.hh"

#include <iostream>

namespace bzd {

class RegexpAsync : public bzd::Regexp
{
public:
	using bzd::Regexp::Regexp;

public:
	template <concepts::asyncInputByteCopyableRange Input>
	bzd::Async<Size> match(Input&& input) noexcept
	{
		auto it = bzd::begin(input);
		auto end = bzd::end(input);

		Context context{regexp_};
		while (!context.regexp.empty())
		{
			auto result = next(bzd::move(context));
			while (result.loop())
			{
				Context::ResultProcess resultProcess{};
				do
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
							resultProcess.maybeError = regexp::Error::noMoreInput;
							break;
						}
					}
					resultProcess = result.valueMutable().process(it, end, resultProcess);
				} while (resultProcess.maybeError && resultProcess.maybeError.value() == regexp::Error::noMoreInput);
				result.update(resultProcess);
			}
			if (!result)
			{
				switch (result.error())
				{
				case regexp::Error::noMoreInput:
				case regexp::Error::noMoreCapture:
					co_return bzd::error::Data("Not enough data");
				case regexp::Error::noMatch:
					co_return bzd::error::Data("No match");
				case regexp::Error::malformed:
					co_return bzd::error::Failure("Regexp malformed");
				}
			}
			context = bzd::move(result).value();
		}
		co_return context.counter;
	}

	template <concepts::asyncInputByteCopyableRange Input, bzd::concepts::outputByteCopyableRange Output>
	bzd::Async<Size> capture(Input&& input, Output&& output) noexcept
	{
		bzd::ranges::Stream oStream{bzd::inPlace, output};
		Bool overflow = false;
		InputStreamCaptureRangeAsync readerCapture{input, oStream, overflow};
		const auto size = co_await !match(readerCapture);
		if (overflow)
		{
			co_return bzd::error::Data("Capture overflow");
		}
		co_return size;
	}

private:
	template <concepts::asyncInputByteCopyableRange Input, bzd::concepts::outputByteCopyableRange Capture>
	class InputStreamCaptureRangeAsync : public InputStreamCaptureRange<Input, Capture>
	{
	public:
		constexpr InputStreamCaptureRangeAsync(Input& input, Capture& capture, Bool& overflow) noexcept :
			InputStreamCaptureRange<Input, Capture>{input, capture, overflow}
		{
		}

		bzd::Async<> next() noexcept { co_return co_await this->range_.get().next(); }
	};
};

} // namespace bzd
