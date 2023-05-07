#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/utility/pattern/matcher/base.hh"
#include "cc/bzd/utility/pattern/pattern.hh"
#include "cc/bzd/utility/regexpr/regexpr.hh"

namespace bzd {

template <concepts::constexprStringView T>
struct Matcher<T>
{
public:
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
		auto [context, processor] = bzd::pattern::impl::make<Range, StringReader, Schema>(pattern, bzd::forward<Args>(args)...);
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
				const auto maybeSize = processor.process(range, fragment);
				if (!maybeSize)
				{
					return bzd::nullopt;
				}
				counter += maybeSize.value();
			}
		}

		return counter;
	}

private:
	class StringReader
	{
	public:
		template <class Range, class Value>
		static constexpr auto process(Range& range, Value& value, [[maybe_unused]] const auto metadata) noexcept
		{
			if constexpr (bzd::concepts::fromString<Range, Value&, decltype(metadata)>)
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
		/// Get the specialization associated with a type.
		template <class Value>
		using Specialization = typename typeTraits::template Matcher<Value>;
	};
};

} // namespace bzd
