#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/pattern/from_string/output_range.hh"
#include "cc/bzd/utility/regexp/async.hh"

namespace bzd {

template <concepts::outputByteCopyableRange Output>
struct FromStream<Output> : public bzd::FromString<Output>
{
	using Metadata = typename bzd::FromString<Output>::Metadata;

	template <concepts::readerAsync Input, class T>
	static bzd::Async<Size> process(Input&& input, T&& output, const Metadata metadata = Metadata{}) noexcept
	{
		bzd::RegexpAsync regexp{metadata.regexp};
		co_return co_await regexp.capture(input, output);
	}
};

} // namespace bzd
