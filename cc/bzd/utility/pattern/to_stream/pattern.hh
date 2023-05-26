#pragma once

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/utility/pattern/async.hh"
#include "cc/bzd/utility/pattern/to_stream/base.hh"

namespace bzd {

template <concepts::constexprStringView Pattern>
struct ToStream<Pattern>
{
public:
	// Note, Args&& here doesn't work for esp32 gcc compiler, the values are garbage, it seems that the temporary
	// is out of scope within the coroutine. I remember seeing a bugzilla about this for gcc.
	// Would be worth trying again with a new version of the compiler.
	// TODO: Try to enable Args&& with an updated esp32 gcc compiler
	template <class... Args>
	static bzd::Async<Size> process(bzd::OStream& stream, const Pattern& pattern, const Args&... args) noexcept
	{
		const auto [parser, processor] = bzd::pattern::impl::makeAsync<bzd::OStream&, Schema>(pattern, args...);

		Size counter{0u};
		// Run-time call
		for (const auto& result : parser)
		{
			if (!result.str.empty())
			{
				co_await !stream.write(result.str.asBytes());
				counter += result.str.size();
			}
			if (result.isMetadata)
			{
				counter += co_await !processor.process(stream, result);
			}
		}

		co_return counter;
	}

private:
	class Schema
	{
	public:
		/// Get the specialization associated with a type.
		template <class Value>
		using Specialization = typename typeTraits::template ToStream<Value>;
	};
};

} // namespace bzd
