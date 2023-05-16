#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/regexp/matcher_brackets.hh"
#include "cc/bzd/utility/regexp/matcher_single_char.hh"
#include "cc/bzd/utility/regexp/types.hh"

namespace bzd {

/// Simple regular expression matcher.
///
/// Supports a limited version of typical regular expressions. it is a single pass algorithm,
/// with no preprocessing which limits its capabilies.
/// Here are the set of features this version supports:
/// - String matching.
/// - Character group [...], negation [^...] and ranges [...-...].
/// - Repetitions: '*', '+' and '?'.
/// - Special characters: '.', '\s', '\w'.
class Regexp
{
public:
	constexpr explicit Regexp(const bzd::StringView regexp) noexcept : regexp_{regexp} {}

private:
	static constexpr Result<Size, regexp::Error> matcher(bzd::StringView regexp, auto& stream) noexcept
	{
		auto it = bzd::begin(stream);
		const auto end = bzd::end(stream);

		const auto matcherProcessor = [&it, &end](auto matcher) -> Result<Size, regexp::Error> {
			Size counter = 0u;
			while (it != end)
			{
				auto result = matcher.match([](bzd::Monostate) -> regexp::Result { return bzd::error::make(regexp::Error::malformed); },
											[&it](auto& matcher) { return matcher(*it); });

				if (result)
				{
					++it;
					++counter;
					if (result.value() == regexp::Success::last)
					{
						return counter;
					}
					else
					{
						continue;
					}
				}
				return bzd::move(result).propagate();
			}
			return bzd::error::make(regexp::Error::noMatch);
		};

		Size counter{0u};
		while (!regexp.empty())
		{
			// Identify the matcher.
			bzd::Variant<bzd::Monostate, regexp::MatcherBrackets, regexp::MatcherSingleChar> matcher;
			if (auto maybeValue = regexp::MatcherBrackets::make(regexp); maybeValue)
			{
				matcher.set<regexp::MatcherBrackets>(maybeValue.value());
			}
			else if (auto maybeValue = regexp::MatcherSingleChar::make(regexp); maybeValue)
			{
				matcher.set<regexp::MatcherSingleChar>(maybeValue.value());
			}
			else
			{
				return bzd::error::make(regexp::Error::malformed);
			}

			// Identify the number of times the matcher should match.
			Size minMatch = 1u;
			Size maxMatch = 1u;
			Size currentMatch = 0u;

			if (regexp.size())
			{
				if (regexp[0] == '*')
				{
					minMatch = 0u;
					maxMatch = 0u;
					regexp.removePrefix(1u);
				}
				else if (regexp[0] == '+')
				{
					minMatch = 1u;
					maxMatch = 0u;
					regexp.removePrefix(1u);
				}
				else if (regexp[0] == '?')
				{
					minMatch = 0u;
					maxMatch = 1u;
					regexp.removePrefix(1u);
				}
			}

			// Match.
			do
			{
				const auto maybeCounter = matcherProcessor(matcher);
				if (!maybeCounter)
				{
					if (maybeCounter.error() == regexp::Error::malformed)
					{
						return bzd::error::make(regexp::Error::malformed);
					}
					break;
				}
				counter += maybeCounter.value();
				++currentMatch;
			} while (maxMatch == 0u || currentMatch < maxMatch);

			if (currentMatch < minMatch)
			{
				return bzd::error::make(regexp::Error::noMatch);
			}
		}

		return counter;
	}

public:
	template <bzd::concepts::inputByteCopyableRange Range>
	[[nodiscard]] constexpr Result<Size, regexp::Error> match(Range&& range) noexcept
	{
		auto stream = bzd::range::makeStream(range);
		return matcher(regexp_, stream);
	}

	template <bzd::concepts::inputByteCopyableRange Range, bzd::concepts::outputByteCopyableRange Output>
	[[nodiscard]] constexpr Result<Size, regexp::Error> capture(Range&& range, Output& output) noexcept
	{
		auto iStream = bzd::range::makeStream(range);
		auto oStream = bzd::range::makeStream(output);
		InputStreamCaptureRange capture{iStream, oStream};
		return matcher(regexp_, capture);
	}

private:
	template <bzd::concepts::outputByteCopyableRange Capture, class Iterator>
	class IteratorCapture : public Iterator
	{
	public:
		constexpr IteratorCapture(Capture& capture, const Iterator& it) noexcept : Iterator{it}, capture_{capture} {}
		constexpr IteratorCapture& operator++() noexcept
		{
			captureByte();
			Iterator::operator++();
			return *this;
		}
		constexpr IteratorCapture operator++(int) noexcept
		{
			IteratorCapture it{*this};
			captureByte();
			Iterator::operator++(0);
			return it;
		}

	private:
		constexpr void captureByte() noexcept
		{
			auto it = capture_.begin();
			if (it != capture_.end())
			{
				*it = Iterator::operator*();
				++it;
			}
		}

	private:
		Capture& capture_;
	};

	template <bzd::concepts::inputByteCopyableRange Range, bzd::concepts::outputByteCopyableRange Capture>
	class InputStreamCaptureRange
	{
	public:
		constexpr InputStreamCaptureRange(Range& range, Capture& capture) noexcept : range_{range}, capture_{capture} {}

	public:
		constexpr auto begin() noexcept { return IteratorCapture{capture_, range_.begin()}; }
		constexpr auto end() noexcept { return range_.end(); }
		constexpr auto size() noexcept { return range_.size(); }

	private:
		Range& range_;
		Capture& capture_;
	};

private:
	bzd::StringView regexp_;
};

} // namespace bzd
