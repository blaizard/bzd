#pragma once

#include "cc/bzd/utility/pattern/async.hh"
#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_stream/pattern.hh"
#include "cc/bzd/utility/pattern/to_string/enum.hh"

namespace bzd {

template <bzd::concepts::isEnum T>
struct ToStream<T> : ToString<T>
{
public:
	template <class... Args>
	static bzd::Async<Size> process(bzd::OStream& stream, const T value) noexcept
	{
		constexpr auto pattern{"{} ({})"_csv};
		const auto [valueTyped, string] = ToString<T>::preprocess(value);
		co_return co_await !ToStream<decltype(pattern)>::process(stream, pattern, string, valueTyped);
	}
};

} // namespace bzd
