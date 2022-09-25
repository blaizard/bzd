#pragma once

#include "cc/bzd/utility/string/base.hh"

namespace bzd::reader::impl {

template <class Iterator>
constexpr bzd::Optional<Iterator> fromString(Iterator first, Iterator last, const bzd::StringView pattern) noexcept
{
	return first;
}

} // namespace bzd::reader::impl

template <bzd::concepts::containerfromString Range, bzd::concepts::constexprStringView T, class... Args>
constexpr auto fromString(Range&& range, const T& pattern, const Args&... args) noexcept
{
	const auto [parser, processor] =
		bzd::format::impl::make<bzd::format::impl::StringFormatter, bzd::format::impl::SchemaFormat>(pattern, args...);

	// Run-time call
	for (const auto& result : parser)
	{
		/*		if (!result.str.empty())
				{
					str.append(result.str);
				}
				if (result.metadata.hasValue())
				{
					processor.process(str, result.metadata.value());
				}
		*/
	}
}

template <class Range>
constexpr auto fromString(Range&& range, const bzd::StringView pattern) noexcept
{
	return bzd::reader::impl(bzd::begin(range), bzd::end(range), pattern);
}
