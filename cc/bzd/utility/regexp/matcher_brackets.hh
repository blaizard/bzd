#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/utility/regexp/types.hh"

namespace bzd::regexp {

class MatcherBrackets
{
private:
	constexpr explicit MatcherBrackets(const bzd::StringView regexp) noexcept : regexp_{regexp} {}

public:
	static constexpr bzd::Optional<MatcherBrackets> make(bzd::StringView& regexp) noexcept
	{
		if (regexp[0] != '[')
		{
			return bzd::nullopt;
		}
		const auto maybePos = regexp.find(']', 1u);
		if (maybePos == bzd::npos)
		{
			return bzd::nullopt;
		}
		MatcherBrackets matcher{regexp.subStr(1u, maybePos - 1u)};
		regexp.removePrefix(maybePos + 1u);
		return matcher;
	}

	constexpr Result operator()(const char c) noexcept
	{
		// Regexpr within the brackets to be tested.matcherProcessor
		auto itRegexpr = bzd::begin(regexp_);
		const auto endRegexpr = bzd::end(regexp_);

		auto isPositiveLookup = true;
		do
		{
			if (itRegexpr == endRegexpr)
			{
				return bzd::error::make(Error::malformed);
			}

			if (*itRegexpr != '^')
			{
				break;
			}

			// If this is a negate bracket.
			isPositiveLookup = !isPositiveLookup;
			++itRegexpr;
		} while (isPositiveLookup == false);

		bzd::Optional<char> previous{};
		bzd::Bool isMatch = false;
		do
		{
			char current = *itRegexpr;
			switch (current)
			{
			case '-':
				++itRegexpr;
				if (previous.empty() || itRegexpr == endRegexpr)
				{
					return bzd::error::make(Error::malformed);
				}
				current = *itRegexpr;
				if (previous.value() >= current)
				{
					return bzd::error::make(Error::malformed);
				}
				isMatch = (previous.value() <= c && current >= c);
				break;
			default:
				isMatch = (current == c);
			}
			++itRegexpr;
			previous = current;
		} while (!isMatch && itRegexpr != endRegexpr);

		if (isMatch == isPositiveLookup)
		{
			return Success::last;
		}
		return bzd::error::make(Error::noMatch);
	}

private:
	bzd::StringView regexp_;
};

} // namespace bzd::regexp
