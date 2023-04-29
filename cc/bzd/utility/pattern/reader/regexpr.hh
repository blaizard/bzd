#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/range.hh"

#include <iostream>

namespace bzd {

/// Simple regular expression matcher.
///
/// Supports a limited version of typical regular expressions. it is a single pass algorithm,
/// with no preprocessing which limits its capabilies.
/// Here are the set of features this version supports:
/// - String matching.
/// - Character group [...], negation [^...] and ranges [...-...].
/// - Repetitions: '*', '+' and '?'.
/// - Special characters: '.', '\s'.
class Regexpr
{
public:
	enum class Error
	{
		malformed,
		noMatch
	};

public:
	constexpr explicit Regexpr(const bzd::StringView regexpr) noexcept : regexpr_{regexpr} {}

private:
	template <bzd::concepts::inputStreamRange Range>
	static constexpr Result<Size, Error> matcher(bzd::StringView regexpr, Range&& range) noexcept
	{
		Size counter{0u};
		while (!regexpr.empty())
		{
			// Identify the matcher.

			struct Matcher
			{
				bzd::typeTraits::AddPointer<Result<Size, Error>(bzd::StringView, Range&&)> processor;
				bzd::StringView regexpr;
			} matcher;

			if (regexpr[0] == '[')
			{
				const auto maybePos = regexpr.find(']', 1u);
				if (maybePos == bzd::npos)
				{
					return bzd::error::make(Error::malformed);
				}
				matcher = Matcher{Regexpr::matcherBrackets<Range>, regexpr.subStr(1u, maybePos - 1u)};
				regexpr.removePrefix(maybePos + 1u);
			}
			else if (regexpr[0] == '\\')
			{
				if (regexpr.size() < 2u)
				{
					return bzd::error::make(Error::malformed);
				}
				matcher = Matcher{Regexpr::matcherSingleChar<Range>, regexpr.subStr(0, 2u)};
				regexpr.removePrefix(2u);
			}
			else
			{
				matcher = Matcher{Regexpr::matcherSingleChar<Range>, regexpr.subStr(0, 1u)};
				regexpr.removePrefix(1u);
			}

			// Identify the numbe of times the matche should match.

			Size minMatch = 1u;
			Size maxMatch = 1u;
			Size currentMatch = 0u;

			if (regexpr.size())
			{
				if (regexpr[0] == '*')
				{
					minMatch = 0u;
					maxMatch = 0u;
					regexpr.removePrefix(1u);
				}
				else if (regexpr[0] == '+')
				{
					minMatch = 1u;
					maxMatch = 0u;
					regexpr.removePrefix(1u);
				}
				else if (regexpr[0] == '?')
				{
					minMatch = 0u;
					maxMatch = 1u;
					regexpr.removePrefix(1u);
				}
			}

			do
			{
				auto maybeCounter = matcher.processor(matcher.regexpr, range);
				if (!maybeCounter)
				{
					if (maybeCounter.error() == Error::malformed)
					{
						return bzd::error::make(Error::malformed);
					}
					break;
				}
				counter += maybeCounter.value();
				++currentMatch;
			} while (maxMatch == 0u || currentMatch < maxMatch);

			if (currentMatch < minMatch)
			{
				return bzd::error::make(Error::noMatch);
			}
		}

		return counter;
	}

	template <bzd::concepts::inputStreamRange Range>
	static constexpr Result<Size, Error> matcherSingleChar(const bzd::StringView regexpr, Range&& range) noexcept
	{
		auto itRange = bzd::begin(range);
		if (itRange != bzd::end(range))
		{
			auto value = regexpr[0];
			if (value == '*' || value == '+' || value == '?')
			{
				return bzd::error::make(Error::malformed);
			}
			else if (value == '.')
			{
				++itRange;
				return 1u;
			}
			else if (value == '\\')
			{
				value = regexpr[1];
				if (value == 's')
				{
					if (*itRange == ' ' || *itRange == '\t' || *itRange == '\r' || *itRange == '\n' || *itRange == '\f')
					{
						++itRange;
						return 1u;
					}
				}
			}
			if (value == *itRange)
			{
				++itRange;
				return 1u;
			}
		}
		return bzd::error::make(Error::noMatch);
	}

	template <bzd::concepts::inputStreamRange Range>
	static constexpr Result<Size, Error> matcherBrackets(const bzd::StringView regexpr, Range&& range) noexcept
	{
		// Value to be tested.
		auto itRange = bzd::begin(range);
		const auto value = *itRange;

		// Regexpr within the brackets to be tested.
		auto itRegexpr = bzd::begin(regexpr);
		const auto endRegexpr = bzd::end(regexpr);

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
				isMatch = (previous.value() <= value && current >= value);
				break;
			default:
				isMatch = (current == value);
			}
			++itRegexpr;
			previous = current;
		} while (!isMatch && itRegexpr != endRegexpr);

		if (isMatch == isPositiveLookup)
		{
			++itRange;
			return 1u;
		}
		return bzd::error::make(Error::noMatch);
	}

public:
	template <bzd::concepts::inputStreamRange Range>
	[[nodiscard]] constexpr Result<Size, Error> match(Range&& range) noexcept
	{
		return matcher(regexpr_, range);
	}

private:
	bzd::StringView regexpr_;
};

} // namespace bzd
