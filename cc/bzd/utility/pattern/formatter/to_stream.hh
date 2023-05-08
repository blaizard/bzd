#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/utility/pattern/async.hh"
#include "cc/bzd/utility/pattern/formatter/to_string.hh"

namespace bzd::format::impl {

// ---- Formatter ----

template <class Range, class T>
concept toStreamFormatterWithMetadata =
	requires(Range& range, const T& value) { toStream(range, value, bzd::typeTraits::declval<const Metadata>()); };

class StreamFormatter
{
public:
	template <class Range, class T>
	static Async<> process(Range& range, const T& value, [[maybe_unused]] const Metadata& metadata) noexcept
	{
		if constexpr (toStreamFormatterWithMetadata<Range, T>)
		{
			co_await !toStream(range, value, metadata);
		}
		else
		{
			co_await !toStream(range, value);
		}
		co_return {};
	}
};

// Specialization of toStream for the native types

template <class T>
requires(concepts::integral<T> || concepts::floatingPoint<T> || (concepts::pointer<T> && !concepts::constructible<bzd::StringView, T>))
Async<> toStream(bzd::OStream& stream, const T& value, const Metadata& metadata) noexcept
{
	bzd::String<80> str{};
	toString(str.assigner(), value, metadata);
	co_await !stream.write(str.asBytes());
	co_return {};
}

inline Async<> toStream(bzd::OStream& stream, const bzd::StringView stringView, const Metadata& metadata) noexcept
{
	co_await !stream.write(processCommon(stringView, metadata).asBytes());
	co_return {};
}

} // namespace bzd::format::impl

// Note, Args&& here doesn't work for esp32 gcc compiler, the values are garbage, it seems that the temporary
// is out of scope within the coroutine. I remember seeing a bugzilla about this for gcc.
// Would be worth trying again with a new version of the compiler.
// TODO: Try to enable Args&& with an updated esp32 gcc compiler
template <bzd::concepts::constexprStringView Pattern, class... Args>
bzd::Async<> toStream(bzd::OStream& stream, const Pattern& pattern, const Args&... args) noexcept
{
	const auto [parser, processor] =
		bzd::pattern::impl::makeAsync<bzd::OStream&, bzd::format::impl::StreamFormatter, bzd::format::impl::SchemaFormat>(pattern, args...);

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			co_await !stream.write(result.str.asBytes());
		}
		if (result.isMetadata)
		{
			co_await !processor.process(stream, result);
		}
	}

	co_return {};
}

inline bzd::Async<> toStream(bzd::OStream& stream, const bzd::interface::String& str) noexcept
{
	co_await !stream.write(str.asBytes());
	co_return {};
}

inline bzd::Async<> toStream(bzd::OStream& stream, const bzd::StringView str) noexcept
{
	co_await !stream.write(str.asBytes());
	co_return {};
}

inline bzd::Async<> toStream(bzd::OStream& stream, const char* const str) noexcept
{
	co_await !stream.write(bzd::StringView{str}.asBytes());
	co_return {};
}
