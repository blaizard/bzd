#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/regexp/regexp.hh"

namespace bzd {

class RegexpAsync : public bzd::Regexp
{
public:
	using bzd::Regexp::Regexp;

public:
	template <class ReaderAsync>
	bzd::Async<Size> match(ReaderAsync&& reader) noexcept
	{
		auto range = co_await !reader.reader();
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
						range = co_await !reader.reader();
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

	/*
		// (wip)
		template <class ReaderAsync>
		bzd::Async<Size> capture(ReaderAsync&& reader, Output&& output) noexcept
		{
			auto iStream = bzd::range::makeStream(range);
			auto oStream = bzd::range::makeStream(output);
			InputStreamCaptureRange capture{iStream, oStream};
			co_return co_await !match(capture);
		}
	*/
};

} // namespace bzd
