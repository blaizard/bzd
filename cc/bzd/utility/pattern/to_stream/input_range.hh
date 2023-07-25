#pragma once

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_string/input_range.hh"

namespace bzd {

template <class T>
requires(concepts::convertible<T, bzd::StringView> || (concepts::syncRange<T> && concepts::inputByteCopyableRange<T>))
struct ToStream<T> : ToString<bzd::StringView>
{
public:
	using Metadata = typename ToString<bzd::StringView>::Metadata;

	template <class U>
	static bzd::Async<Size> process(bzd::OStream& stream, U&& value, const Metadata metadata = Metadata{}) noexcept
	{
		if constexpr (concepts::convertible<U, bzd::StringView>)
		{
			co_return co_await processBase(stream, bzd::StringView{value}, metadata);
		}
		else
		{
			co_return co_await processBase(stream, value, metadata);
		}
	}

private:
	template <concepts::contiguousRange U>
	static bzd::Async<Size> processBase(bzd::OStream& stream, U&& value, const Metadata metadata) noexcept
	{
		bzd::Span<const bzd::Byte> span{reinterpret_cast<const bzd::Byte*>(&(*bzd::begin(value))), bzd::size(value)};

		if (metadata.isPrecision)
		{
			const auto size = bzd::min(metadata.precision, span.size());
			co_await !stream.write(span.subSpan(0, size));
			co_return size;
		}
		else
		{
			co_await !stream.write(span);
			co_return span.size();
		}
	}

	template <class U>
	static bzd::Async<Size> processBase(bzd::OStream& stream, U&& value, const Metadata metadata) noexcept
	{
		bzd::Size count{0u};
		auto it = bzd::begin(value);
		auto end = bzd::end(value);

		if (metadata.isPrecision)
		{
			for (; it != end && count < metadata.precision; ++count, ++it)
			{
				co_await !stream.write(bzd::Span<const bzd::Byte>{reinterpret_cast<const bzd::Byte*>(&(*it)), 1u});
			}
		}
		else
		{
			for (; it != end; ++count, ++it)
			{
				co_await !stream.write(bzd::Span<const bzd::Byte>{reinterpret_cast<const bzd::Byte*>(&(*it)), 1u});
			}
		}
		co_return count;
	}
};

} // namespace bzd
