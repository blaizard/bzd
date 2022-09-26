#pragma once

#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/utility/string/base.hh"
#include "cc/bzd/utility/string/reader/integral.hh"

namespace bzd {
template <class Range>
using FromStringReturnType = bzd::Optional<typename bzd::typeTraits::Iterator<Range>::Type>;
}

namespace bzd::reader::impl {

struct Metadata
{
	bzd::Size index{0u};
	bzd::StringView pattern{};
};

// ---- Formatter ----

template <class Range, class T>
concept fromStringFormatterWithMetadata = requires(Range& range, T& value)
{
	::fromString(range, value, bzd::typeTraits::declval<const Metadata>());
};

template <class Range, class T>
concept fromStringFormatter = requires(Range& range, T& value)
{
	::fromString(range, value);
};

class StringReader
{
public:
	template <class Range, class T>
	static constexpr auto process(Range& range, T& value, [[maybe_unused]] const Metadata& metadata) noexcept
	{
		if constexpr (fromStringFormatterWithMetadata<Range, T>)
		{
			return fromString(range, value, metadata);
		}
		else //if constexpr (fromStringFormatter<Range, T>)
		{
			return fromString(range, value);
		}
	}
};

class Schema
{
public:
	using Metadata = bzd::reader::impl::Metadata;

	template <class Adapter>
	static constexpr void parse(Metadata&, bzd::StringView&) noexcept
	{
	}

	template <class Adapter, class ValueType>
	static constexpr void check(const Metadata&) noexcept
	{
	}
};

template <class Iterator>
constexpr bzd::Optional<Iterator> fromString(Iterator first, Iterator, const bzd::StringView) noexcept
{
	return first;
}

} // namespace bzd::reader::impl

template <bzd::concepts::containerFromString Range, bzd::concepts::constexprStringView T, class... Args>
constexpr bzd::FromStringReturnType<Range> fromString(Range&&, const T& pattern, Args&... args) noexcept
{
	auto [parser, processor] =
		bzd::format::impl::make<Range, bzd::reader::impl::StringReader, bzd::reader::impl::Schema>(pattern, args...);

	// Run-time call
	for (const auto& result : parser)
	{
		(void)result;
	}

	return bzd::nullopt;
}

template <class Range>
constexpr auto fromString(Range&& range, const bzd::StringView pattern) noexcept
{
	return bzd::reader::impl::fromString(bzd::begin(range), bzd::end(range), pattern);
}
