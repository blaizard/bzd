#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

template <Size capacity = 100u>
bzd::Async<String<capacity>> read(bzd::IStream& in) noexcept
{
	String<capacity> string;
	string.resize(capacity);
	const auto result = co_await !in.readToBuffer(string.asBytesMutable());
	string.resize(result.size());
	co_return string;
}

} // namespace bzd::test
