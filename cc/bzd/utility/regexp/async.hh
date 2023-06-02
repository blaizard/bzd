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
	template <concepts::readerAsync Input>
	bzd::Async<Size> match(Input&& input) noexcept
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
						auto maybeRange = co_await input.reader();
						if (maybeRange)
						{
							range = bzd::move(maybeRange.valueMutable());
							it = bzd::begin(range);
							end = bzd::end(range);
						}
						else if (maybeRange.error().getType() != bzd::ErrorType::eof)
						{
							co_return bzd::move(maybeRange).propagate();
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

	template <concepts::readerAsync Input, bzd::concepts::outputByteCopyableRange Output>
	bzd::Async<Size> capture(Input&& input, Output&& output) noexcept
	{
		bzd::range::Stream oStream{bzd::inPlace, output};
		Bool overflow = false;
		ReaderAsyncCaptureRange readerCapture{input, oStream, overflow};
		const auto size = co_await !match(readerCapture);
		if (overflow)
		{
			co_return bzd::error::Data("Capture overflow");
		}
		co_return size;
	}

private:
	template <concepts::readerAsync Input, bzd::concepts::outputByteCopyableRange Capture>
	class ReaderAsyncCaptureRange
	{
	private:
		using ReaderAsyncType = typename bzd::typeTraits::InvokeResult<decltype(&Input::reader), Input>::Value;

	public:
		constexpr ReaderAsyncCaptureRange(Input& input, Capture& capture, Bool& overflow) noexcept :
			input_{input}, capture_{capture}, overflow_{overflow}
		{
		}

		bzd::Async<InputStreamCaptureRange<ReaderAsyncType, Capture>> reader() noexcept
		{
			auto range = co_await !input_.reader();
			co_return InputStreamCaptureRange<ReaderAsyncType, Capture>{bzd::move(range), capture_, overflow_};
		}

	private:
		Input& input_;
		Capture& capture_;
		bzd::ReferenceWrapper<Bool> overflow_;
	};
};

} // namespace bzd
