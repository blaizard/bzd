#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/channel.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/utility/regexp/regexp.hh"

namespace bzd {

class RegexpAsync : public bzd::Regexp
{
public:
	using bzd::Regexp::Regexp;

public:
	template <concepts::readerAsync ReaderAsync>
	bzd::Async<Size> match(ReaderAsync&& input) noexcept
	{
		auto range = co_await !input.reader();
		auto it = bzd::begin(range);
		auto end = bzd::end(range);

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
						range = co_await !input.reader();
						it = bzd::begin(range);
						end = bzd::end(range);
						if (it == end)
						{
							co_return bzd::error::Data("Not enough data");
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

	template <concepts::readerAsync ReaderAsync, bzd::concepts::outputByteCopyableRange Output>
	bzd::Async<Size> capture(ReaderAsync&& input, Output&& output) noexcept
	{
		auto oStream = bzd::range::makeStream(output);
		ReaderAsyncCaptureRange readerCapture{input, oStream};
		co_return co_await !match(readerCapture);
	}

private:
	template <concepts::readerAsync ReaderAsync, bzd::concepts::outputByteCopyableRange Capture>
	class ReaderAsyncCaptureRange
	{
	private:
		using ReaderAsyncType = typename bzd::typeTraits::InvokeResult<decltype(&ReaderAsync::reader), ReaderAsync>::Value;

	public:
		constexpr ReaderAsyncCaptureRange(ReaderAsync& input, Capture& capture) noexcept : input_{input}, capture_{capture} {}

		bzd::Async<InputStreamCaptureRange<ReaderAsyncType, Capture>> reader() noexcept
		{
			auto range = co_await !input_.reader();
			co_return InputStreamCaptureRange<ReaderAsyncType, Capture>{bzd::move(range), capture_};
		}

	private:
		ReaderAsync& input_;
		Capture& capture_;
	};
};

} // namespace bzd
