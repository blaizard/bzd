#pragma once

#include "cc/bzd.hh"

namespace bzd::components::generic {

template <class T>
class Comparison
{
public:
	using Iterator = bzd::typeTraits::RangeIterator<T>;

public:
	constexpr Comparison(const T& range) noexcept : first_{bzd::begin(range)}, last_{bzd::end(range)} {}

	constexpr bool operator()(const typeTraits::RangeValue<T>& element, const bzd::Byte c) const noexcept
	{
		const auto actualElement = element.first;
		if (index_ >= bzd::size(actualElement))
		{
			return true;
		}
		return actualElement.at(index_) < static_cast<char>(c);
	}

	constexpr bool operator()(const bzd::Byte c, const typeTraits::RangeValue<T>& element) const noexcept
	{
		const auto actualElement = element.first;
		if (index_ >= bzd::size(actualElement))
		{
			return false;
		}
		return static_cast<char>(c) < actualElement.at(index_);
	}

	/// Process a new character.
	///
	/// \return An error result if there is no match.
	///         An empty result if there are matched but no full match.
	///         An iterator if there is a full match.
	constexpr bzd::Tuple<Iterator, Iterator> process(const bzd::Byte c) noexcept
	{
		auto [it, lastUpdated] = bzd::algorithm::equalRange(first_, last_, c, *this);
		++index_;
		first_ = it;
		last_ = lastUpdated;

		return {bzd::inPlace, first_, last_};
	}

private:
	Iterator first_;
	Iterator last_;
	bzd::Size index_{0};
};

class Screen
{
public:
	constexpr Screen(bzd::OStream& out, const Size maxLines = 10u) noexcept : out_{out}, maxLines_{maxLines} {}

	bzd::Async<> print(bzd::StringView view) noexcept
	{
		for (const auto b : view.asBytes())
		{
			co_await !print(b);
		}
		co_return {};
	}

	// Only print the visible characters.
	bzd::Async<> print(const bzd::Byte b) noexcept
	{
		if (static_cast<int>(b) >= 32 && static_cast<int>(b) <= 126)
		{
			co_await !bzd::toStream(out_, static_cast<const char>(b));
			++cursorX_;
		}
		co_return {};
	}

	bzd::Async<> printSuggestions(auto it, auto end) noexcept
	{
		Size count = 0u;
		for (; it != end && count < maxLines_; ++it, ++count)
		{
			co_await !bzd::toStream(out_, "\n\033[K{}"_csv, it->first);
		}
		const auto newDirtyLines = count;
		for (; count < dirtyLines_; ++count)
		{
			co_await !bzd::toStream(out_, "\n\033[K"_csv);
		}
		dirtyLines_ = newDirtyLines;
		// Reset the cursor to its initial position.
		if (count)
		{
			co_await !bzd::toStream(out_, "\033[{}A"_csv, count);
		}
		co_await !bzd::toStream(out_, "\r\033[{}C"_csv, cursorX_);
		co_return {};
	}

private:
	bzd::OStream& out_;
	const Size maxLines_;
	Size cursorX_{0};
	Size dirtyLines_{0};
};

template <class Context>
class Console
{
public:
	constexpr explicit Console(Context& context) noexcept : context_{context}, logger_{context_.config.out} {}

	bzd::Async<> help() { co_return {}; }

	bzd::Async<> power() { co_return {}; }

	bzd::Async<> printMenu()
	{
		using Action = bzd::FunctionRef<bzd::Async<>(void)>;
		using Map = bzd::interface::Map<bzd::StringView, void*>;
		using Output = bzd::Variant<Action, Map>;

		bzd::Map<bzd::StringView, Output, 4u> entries{{"help"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"io"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"logging"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"power"_sv, Action::toMember<Console, &Console::power>(*this)}};

		Screen screen{context_.config.out};
		co_await !screen.print("> ");

		auto first = entries.begin();
		auto last = entries.end();

		co_await !screen.printSuggestions(first, last);

		auto& in = context_.config.in;

		Comparison comparison{entries};

		bzd::Array<bzd::Byte, 1u> buffer;
		auto generator = in.reader(buffer.asBytesMutable());
		auto it = co_await !generator.begin();
		while (it != generator.end())
		{
			auto itByte = bzd::begin(*it);
			auto [first, last] = comparison.process(*itByte);
			co_await !screen.print(*itByte);

			// There is no match.
			/*if (first == last)
			{
				co_await !bzd::toStream(out, "No command found"_csv);
			}
			// There is a match.
			else if (bzd::distance(first, last) == 1u)
			{
				co_await !bzd::toStream(out, "MATCH! {}\n"_csv, first->first);
			}
			else*/
			{
				co_await !screen.printSuggestions(first, last);
			}

			co_await !++it;
		}
		// co_await !bzd::toStream(out, "Excellent choice, you chose: {}\n"_csv, choice);

		co_return {};
	}

	bzd::Async<> run()
	{
		co_await !printMenu();

		co_return {};
	}

private:
	Context& context_;
	bzd::Logger logger_;
};

} // namespace bzd::components::generic
