#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/views/stream.hh"
#include "cc/bzd/container/reference_wrapper.hh"
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
/// - Special characters: '.', '\s', '\S', '\w', '\W'.
class Regexp
{
public:
	constexpr explicit Regexp(const bzd::StringView regexp) noexcept : regexp_{regexp} {}

protected:
	struct Context
	{
		bzd::StringView regexp;
		bzd::Variant<bzd::Monostate, regexp::MatcherBrackets, regexp::MatcherSingleChar> matcher{};
		Size minMatch{1u};
		Size maxMatch{1u};
		Size currentMatch{0u};
		Size counter{0u};

		struct ResultProcess
		{
			Size counter{0u};
			Optional<regexp::Error> maybeError{};
		};

		template <concepts::inputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
		constexpr ResultProcess process(Iterator& it,
										const Sentinel& end,
										ResultProcess resultProcess = ResultProcess{0u, bzd::nullopt}) noexcept
		{
			while (it != end)
			{
				auto result = matcher.match([](bzd::Monostate) -> regexp::Result { return bzd::error::make(regexp::Error::malformed); },
											[&it](auto& matcher) { return matcher(*it); });
				if (result)
				{
					++it;
					++resultProcess.counter;
					if (result.value() == regexp::Success::last)
					{
						return resultProcess;
					}
					continue;
				}
				resultProcess.maybeError = result.error();
				return resultProcess;
			}
			resultProcess.maybeError = regexp::Error::noMoreInput;
			return resultProcess;
		}
	};

	class ContextResult : public bzd::Result<Context, regexp::Error>
	{
	public:
		using bzd::Result<Context, regexp::Error>::Result;

		/// Update the counter of the context.
		constexpr void update(const Context::ResultProcess result) noexcept
		{
			auto& context = this->valueMutable();
			if (result.maybeError)
			{
				if (result.maybeError.value() == regexp::Error::malformed)
				{
					*this = bzd::error::make(regexp::Error::malformed);
				}
				else if (context.currentMatch < context.minMatch)
				{
					*this = bzd::error::make(regexp::Error::noMatch);
				}
				else
				{
					// Condition to exit.
					context.maxMatch = context.currentMatch = 1u;
				}
			}
			else
			{
				context.counter += result.counter;
				++context.currentMatch;
			}
		}

		constexpr bzd::Bool loop() const noexcept
		{
			if (hasError())
			{
				return false;
			}
			const auto& context = this->value();
			return (context.maxMatch == 0u || context.currentMatch < context.maxMatch);
		}
	};

	constexpr ContextResult next(Context&& context) noexcept
	{
		// Identify the matcher.
		if (auto maybeValue = regexp::MatcherBrackets::make(context.regexp); maybeValue)
		{
			context.matcher.set<regexp::MatcherBrackets>(maybeValue.value());
		}
		else if (auto maybeValue = regexp::MatcherSingleChar::make(context.regexp); maybeValue)
		{
			context.matcher.set<regexp::MatcherSingleChar>(maybeValue.value());
		}
		else
		{
			return bzd::error::make(regexp::Error::malformed);
		}

		// Identify the number of times the matcher should match.
		context.currentMatch = 0u;
		context.minMatch = 1u;
		context.maxMatch = 1u;
		if (context.regexp.size())
		{
			if (context.regexp[0] == '*')
			{
				context.minMatch = 0u;
				context.maxMatch = 0u;
				context.regexp.removePrefix(1u);
			}
			else if (context.regexp[0] == '+')
			{
				context.minMatch = 1u;
				context.maxMatch = 0u;
				context.regexp.removePrefix(1u);
			}
			else if (context.regexp[0] == '?')
			{
				context.minMatch = 0u;
				context.maxMatch = 1u;
				context.regexp.removePrefix(1u);
			}
		}

		return context;
	}

public:
	template <bzd::concepts::inputByteCopyableRange Range>
	[[nodiscard]] constexpr Result<Size, regexp::Error> match(Range&& range) noexcept
	{
		auto it = bzd::begin(range);
		const auto end = bzd::end(range);

		Context context{regexp_};
		while (!context.regexp.empty())
		{
			auto result = next(bzd::move(context));
			while (result.loop())
			{
				const auto resultProcess = result.valueMutable().process(it, end);
				result.update(resultProcess);
			}
			if (!result)
			{
				return bzd::move(result).propagate();
			}
			context = bzd::move(result).value();
		}
		return context.counter;
	}

	template <bzd::concepts::inputByteCopyableRange Range, bzd::concepts::outputByteCopyableRange Output>
	[[nodiscard]] constexpr Result<Size, regexp::Error> capture(Range&& range, Output&& output) noexcept
	{
		bzd::range::Stream iStream{bzd::inPlace, range};
		bzd::range::Stream oStream{bzd::inPlace, output};
		Bool overflow = false;
		InputStreamCaptureRange capture{bzd::move(iStream), oStream, overflow};
		auto result = match(capture);
		if (overflow)
		{
			return bzd::error::make(regexp::Error::noMoreCapture);
		}
		return bzd::move(result);
	}

protected:
	template <bzd::concepts::outputByteCopyableRange Capture, class Iterator>
	requires(!bzd::concepts::reference<Capture>)
	class IteratorCapture : public Iterator
	{
	public:
		constexpr IteratorCapture(Capture& capture, const Iterator& it, Bool& overflow) noexcept :
			Iterator{it}, capture_{capture}, overflow_{overflow}
		{
		}
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
			auto it = capture_->begin();
			if (it != capture_->end())
			{
				*it = Iterator::operator*();
				++it;
			}
			else
			{
				overflow_.get() = true;
			}
		}

	private:
		bzd::ReferenceWrapper<Capture> capture_;
		bzd::ReferenceWrapper<Bool> overflow_;
	};

	template <bzd::concepts::inputByteCopyableRange Range, bzd::concepts::outputByteCopyableRange Capture>
	class InputStreamCaptureRange
	{
	public:
		constexpr InputStreamCaptureRange(Range&& range, Capture& capture, Bool& overflow) noexcept :
			range_{bzd::move(range)}, capture_{capture}, overflow_{overflow}
		{
		}

		InputStreamCaptureRange(const InputStreamCaptureRange&) = delete;
		InputStreamCaptureRange& operator=(const InputStreamCaptureRange&) = delete;
		InputStreamCaptureRange(InputStreamCaptureRange&&) = default;
		InputStreamCaptureRange& operator=(InputStreamCaptureRange&&) = default;
		~InputStreamCaptureRange() = default;

	public:
		constexpr auto begin() noexcept { return IteratorCapture{capture_.get(), range_.begin(), overflow_}; }
		constexpr auto end() noexcept { return range_.end(); }
		constexpr auto size() noexcept { return range_.size(); }

	private:
		Range range_;
		bzd::ReferenceWrapper<Capture> capture_;
		bzd::ReferenceWrapper<Bool> overflow_;
	};

protected:
	bzd::StringView regexp_;
};

} // namespace bzd
