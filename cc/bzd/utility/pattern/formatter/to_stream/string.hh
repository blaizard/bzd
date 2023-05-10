#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/utility/pattern/formatter/to_stream/base.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/string.hh"

namespace bzd {

template <class T>
requires(concepts::convertible<T, bzd::StringView> || concepts::convertible<T, bzd::interface::String>)
struct FormatterAsync<T> : Formatter<T>
{
public:
	using Metadata = typename Formatter<T>::Metadata;

	static bzd::Async<> toStream(bzd::OStream& stream, const bzd::StringView value, const Metadata metadata = Metadata{}) noexcept
	{
		co_await !toStreamBase(stream, value, metadata);
		co_return {};
	}

	static bzd::Async<> toStream(bzd::OStream& stream, const bzd::interface::String& value, const Metadata metadata = Metadata{}) noexcept
	{
		co_await !toStreamBase(stream, value, metadata);
		co_return {};
	}

private:
	template <class U>
	static bzd::Async<> toStreamBase(bzd::OStream& stream, const U& value, const Metadata metadata) noexcept
	{
		if (metadata.isPrecision)
		{
			const auto size = bzd::min(metadata.precision, value.size());
			co_await !stream.write(value.asBytes().subSpan(0, size));
		}
		else
		{
			co_await !stream.write(value.asBytes());
		}
		co_return {};
	}
};

} // namespace bzd
