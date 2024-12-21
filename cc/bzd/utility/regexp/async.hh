#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/utility/regexp/regexp.hh"

namespace bzd {

class RegexpAsync : public bzd::Regexp
{
public:
	using bzd::Regexp::Regexp;

public:
	template <concepts::generatorInputByteCopyableRange Generator>
	bzd::Async<Size> match(Generator&& generator) noexcept
	{
		Context context{regexp_};
		while (!context.regexp.empty())
		{
			auto result = next(bzd::move(context));
			while (result.loop())
			{
				Context::ResultProcess resultProcess{};
				resultProcess.maybeError = regexp::Error::noMoreInput;

				co_await !bzd::async::forEach(generator, [&](auto& range) -> bool {
					auto it = bzd::begin(range);
					auto end = bzd::end(range);
					if (it == end)
					{
						return true; // no more data, fetch another buffer.
					}
					resultProcess.maybeError.reset();
					resultProcess = result.valueMutable().process(it, end, resultProcess);
					return resultProcess.maybeError && resultProcess.maybeError.value() == regexp::Error::noMoreInput;
				});

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

	template <concepts::generatorInputByteCopyableRange Generator, bzd::concepts::outputByteCopyableRange Output>
	bzd::Async<Size> capture(Generator&& generator, Output&& output) noexcept
	{
		bzd::ranges::Stream oStream{bzd::inPlace, output};
		Bool overflow = false;
		auto wrappedGenerator = generatorCaptureWrapper(generator, oStream, overflow);
		const auto size = co_await !match(wrappedGenerator);
		if (overflow)
		{
			co_return bzd::error::Data("Capture overflow");
		}
		co_return size;
	}

private:
	template <concepts::generatorInputByteCopyableRange Generator, bzd::concepts::outputByteCopyableRange Capture>
	bzd::Generator<InputStreamCaptureRange<typename Generator::ResultType::Value, Capture>> generatorCaptureWrapper(Generator& generator,
																													Capture& capture,
																													Bool& overflow) noexcept
	{
		auto it = co_await !generator.begin();
		while (it != generator.end())
		{
			co_yield InputStreamCaptureRange<typename Generator::ResultType::Value, Capture>{*it, capture, overflow};
			co_await !++it;
		}
	}
};

} // namespace bzd
