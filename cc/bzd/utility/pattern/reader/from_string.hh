#pragma once

#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/utility/pattern/pattern.hh"
#include "cc/bzd/utility/pattern/reader/integral.hh"

namespace bzd {
template <concepts::range Range>
using FromStringReturnType = bzd::Optional<typename bzd::typeTraits::Range<Range>::Iterator>;
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
		else
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
	static constexpr void parse(Metadata&, bzd::StringView& pattern) noexcept
	{
		pattern.removePrefix(1);
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

/// Try to match a range with a pattern.
template <bzd::concepts::containerFromString Range>
constexpr bzd::FromStringReturnType<Range> match(Range&&, const bzd::StringView) noexcept
{
	return bzd::nullopt;
}

} // namespace bzd::reader::impl

template <bzd::concepts::containerFromString Range, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr bzd::FromStringReturnType<Range> fromString(Range&& range, const Pattern& pattern, Args&... args) noexcept
{
	auto [context, processor] =
		bzd::pattern::impl::make<Range, bzd::reader::impl::StringReader, bzd::reader::impl::Schema>(pattern, args...);

	// Run-time call
	for (const auto& fragment : context)
	{
		if (!fragment.str.empty())
		{
			// str.append(result.str);
		}
		if (fragment.isMetadata)
		{
			processor.process(range, fragment.metadata);
		}
	}

	return bzd::nullopt;
}

template <class Range>
constexpr auto fromString(Range&& range, const bzd::StringView pattern) noexcept
{
	return bzd::reader::impl::fromString(bzd::begin(range), bzd::end(range), pattern);
}
