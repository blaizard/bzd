#pragma once

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/utility/pattern/async.hh"
#include "cc/bzd/utility/pattern/formatter/to_stream/base.hh"

namespace bzd {

template <concepts::constexprStringView Pattern>
struct FormatterAsync<Pattern>
{
public:
	// Note, Args&& here doesn't work for esp32 gcc compiler, the values are garbage, it seems that the temporary
	// is out of scope within the coroutine. I remember seeing a bugzilla about this for gcc.
	// Would be worth trying again with a new version of the compiler.
	// TODO: Try to enable Args&& with an updated esp32 gcc compiler
	template <class... Args>
	static bzd::Async<> toStream(bzd::OStream& stream, const Pattern& pattern, const Args&... args) noexcept
	{
		const auto [parser, processor] = bzd::pattern::impl::makeAsync<bzd::OStream&, StreamFormatter, Schema>(pattern, args...);

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

private:
	class StreamFormatter
	{
	public:
		template <class... Args>
		static bzd::Async<> process(Args&&... args) noexcept
		{
			co_await !::bzd::toStream(bzd::forward<Args>(args)...);
			co_return {};
		}
	};

	class Schema
	{
	public:
		/// Get the specialization associated with a type.
		template <class Value>
		using Specialization = typename typeTraits::template FormatterAsync<Value>;
	};
};

} // namespace bzd
