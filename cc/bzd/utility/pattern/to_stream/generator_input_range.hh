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
		co_await !bzd::async::forEach(generator, [&](const auto& data) -> bzd::Async<Bool> {
			const auto size = co_await !bzd::toStream(stream, data, metadata);
			count += size;
			if (metadata.precision <= size)
			{
				co_return false;
			}
			metadata.precision -= size;
			co_return true;
		});
		co_return count;
	}
};

} // namespace bzd
