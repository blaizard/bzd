#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_stream/input_range.hh"

namespace bzd {

template <concepts::generatorInputByteCopyableRange Generator>
struct ToStream<Generator> : ToStream<bzd::StringView>
{
public:
	using Metadata = typename ToStream<bzd::StringView>::Metadata;

	static bzd::Async<Size> process(bzd::OStream& stream, Generator& generator, Metadata metadata = Metadata{}) noexcept
	{
		bzd::Size count{0u};
		auto it = co_await !generator.begin();
		while (it != generator.end())
		{
			const auto size = co_await !bzd::toStream(stream, *it, metadata);
			count += size;
			if (metadata.precision <= size)
			{
				break;
			}
			metadata.precision -= size;
			co_await !++it;
		}
		co_return count;
	}
};

} // namespace bzd
