#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_stream/input_range.hh"

namespace bzd {

template <concepts::byteCopyableGenerator Generator>
struct ToStream<Generator> : ToStream<bzd::StringView>
{
public:
	using Metadata = typename ToStream<bzd::StringView>::Metadata;

	static bzd::Async<Size> process(bzd::OStream& stream, Generator& generator, Metadata metadata = Metadata{}) noexcept
	{
		bzd::Size count{0u};
		auto it = co_await !generator.begin();
		if (metadata.isPrecision)
		{
			while (it != generator.end() && metadata.precision <= count)
			{
				co_await !stream.write(bzd::Span<const bzd::Byte>{reinterpret_cast<const bzd::Byte*>(&(*it)), 1u});
				++count;
				co_await !++it;
			}
		}
		else
		{
			while (it != generator.end())
			{
				co_await !stream.write(bzd::Span<const bzd::Byte>{reinterpret_cast<const bzd::Byte*>(&(*it)), 1u});
				++count;
				co_await !++it;
			}
		}

		co_return count;
	}
};

} // namespace bzd
