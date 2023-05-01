#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/utility/pattern/pattern.hh"
#include "cc/bzd/utility/pattern/reader/base.hh"
#include "cc/bzd/utility/pattern/reader/regexpr.hh"

#include <iostream>

namespace bzd::reader::impl {

// ---- Matcher ----

class StringReader
{
public:
	template <class Range, class T>
	static constexpr auto process(Range& range, T& value, [[maybe_unused]] const FromStringMetadata& metadata) noexcept
	{
		if constexpr (bzd::concepts::fromString<Range, T&, const FromStringMetadata&>)
		{
			return ::bzd::fromString(range, value, metadata);
		}
		else
		{
			return ::bzd::fromString(range, value);
		}
	}
};

class Schema
{
public:
	using Metadata = bzd::FromStringMetadata;

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

} // namespace bzd::reader::impl

namespace bzd {

template <concepts::constexprStringView T, class... Ts>
struct Matcher<T, Ts...>
{
	// Proposed syntax:
	// - Matches the string, unless:
	//    - it starts with `[` -> then it matches one of chars.
	//    - it starts with `(` -> then it matches one of the sequence.
	//    - it starts with `{` -> then it assign the match to a variable.
	//    - `+` means one or more.
	//    - `*` means 0 or more.
	//    - `.` means any character.
	// For example:
	// - "HTTP{:f}\s+{:i}\s+{[A-Z]*}"
	// fromString(myString, ".*{}.*"_csv, )
	template <bzd::concepts::inputStreamRange Range, class... Args>
	static constexpr Optional<Size> fromString(Range&& range, const T& pattern, Args&&... args) noexcept
	{
		auto [context, processor] =
			bzd::pattern::impl::make<Range, bzd::reader::impl::StringReader, bzd::reader::impl::Schema>(pattern,
																										bzd::forward<Args>(args)...);
		Size counter{0u};

		// Run-time call
		for (const auto& fragment : context)
		{
			if (!fragment.str.empty())
			{
				bzd::Regexpr regexpr{fragment.str};
				const auto maybeSize = regexpr.match(range);
				if (!maybeSize)
				{
					return bzd::nullopt;
				}
				counter += maybeSize.value();
			}
			if (fragment.isMetadata)
			{
				const auto maybeSize = processor.process(range, fragment.metadata);
				if (!maybeSize)
				{
					return bzd::nullopt;
				}
				counter += maybeSize.value();
			}
		}

		return counter;
	}
};

template <concepts::outputStreamRange Output>
struct Matcher<Output>
{
	template <bzd::concepts::inputStreamRange Range>
	static constexpr Optional<Size> fromString(Range&&, Output&, const FromStringMetadata&) noexcept
	{
		return 0u;
	}

	template <class Adapter, class ValueType>
	static constexpr void check(const FromStringMetadata&) noexcept
	{
		// TO BE IMPLEMENTED.
	}
};

} // namespace bzd
