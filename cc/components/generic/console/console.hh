#pragma once

#include "cc/bzd.hh"
#include "cc/components/generic/console/types.hh"

namespace bzd::components::generic {

class Suggestions
{
public:
	using Iterator = bzd::typeTraits::RangeIterator<Map>;
	using Value = bzd::typeTraits::RangeValue<Map>;

public:
	constexpr Suggestions(const Map& map) noexcept : map_{map} {}

	/// Process a new character.
	///
	/// \return An error result if there is no match.
	///         An empty result if there are matched but no full match.
	///         An iterator if there is a full match.
	constexpr void process(const bzd::StringView view) noexcept
	{
		struct Compare
		{
			constexpr Bool operator()(const bzd::StringView lhs, const Value& rhs) const noexcept
			{
				return lhs.subStr(0, rhs.first.size()) < rhs.first.subStr(0, lhs.size());
			}

			constexpr Bool operator()(const Value& lhs, const bzd::StringView rhs) const noexcept
			{
				return lhs.first.subStr(0, rhs.size()) < rhs.subStr(0, lhs.first.size());
			}
		};

		auto [first, last] = bzd::algorithm::equalRange(map_, view, Compare{});
		first_ = first;
		last_ = last;
	}

	constexpr auto begin() const noexcept { return first_; }

	constexpr auto end() const noexcept { return last_; }

	constexpr auto size() const noexcept { return bzd::distance(first_, last_); }

	constexpr bzd::Optional<bzd::StringView> first() const noexcept
	{
		if (first_ != last_)
		{
			return first_->first;
		}
		return bzd::nullopt;
	}

private:
	const Map& map_;
	Iterator first_{bzd::begin(map_)};
	Iterator last_{bzd::end(map_)};
};

class Screen
{
public:
	constexpr Screen(bzd::OStream& out, const Size promptSize = 0u, const Size maxLines = 10u) noexcept :
		out_{out}, promptSize_{promptSize}, maxLines_{maxLines}
	{
	}

	bzd::Async<> setPrompt(bzd::StringView prompt) noexcept
	{
		promptSize_ = prompt.size();
		co_await !bzd::toStream(out_, "\r\033[K{}"_csv, prompt);
		co_return {};
	}

	// Only print the visible characters.
	bzd::Async<> append(const bzd::Byte b) noexcept
	{
		if (static_cast<int>(b) >= 32 && static_cast<int>(b) <= 126)
		{
			co_await !bzd::toStream(out_, static_cast<char>(b));
			command_ += static_cast<char>(b);
		}
		co_return {};
	}

	// Remove the last character.
	bzd::Async<Bool> back() noexcept
	{
		if (!command_.empty())
		{
			co_await !bzd::toStream(out_, "\b \b"_csv);
			command_.popBack();
			co_return true;
		}
		co_return false;
	}

	bzd::Async<> printSuggestions(auto& range) noexcept
	{
		Size count = 0u;
		auto it = bzd::begin(range);
		const auto end = bzd::end(range);
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
		co_await !bzd::toStream(out_, "\r\033[{}C"_csv, promptSize_ + command_.size());
		co_return {};
	}

	bzd::Async<> clear() noexcept
	{
		for (Size count = 0u; count < dirtyLines_; ++count)
		{
			co_await !bzd::toStream(out_, "\n\033[K"_csv);
		}
		// Reset the cursor to its initial position.
		if (dirtyLines_)
		{
			co_await !bzd::toStream(out_, "\033[{}A\r\033[{}C"_csv, dirtyLines_, promptSize_ + command_.size());
		}
		dirtyLines_ = 0u;
		co_return {};
	}

	constexpr bzd::StringView getCurrentCommand() const noexcept { return command_.asSpan(); }

	// Update the current command being displayed.
	bzd::Async<> set(const bzd::StringView command) noexcept
	{
		if (!command_.empty())
		{
			// Clear the previous command.
			co_await !bzd::toStream(out_, "\033[{}D\033[K"_csv, command_.size());
		}
		command_ = command;
		co_await !bzd::toStream(out_, command_);
		co_return {};
	}

	Screen makeNested() noexcept
	{
		Screen screen = *this;
		screen.promptSize_ += command_.size();
		screen.command_.clear();
		return screen;
	}

private:
	bzd::OStream& out_;
	Size promptSize_{};
	const Size maxLines_;
	Size dirtyLines_{0};
	bzd::String<64> command_{};
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
		bzd::Map<bzd::StringView, Output, 5u> entries{{"help"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"executor"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"io"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"logging"_sv, Action::toMember<Console, &Console::help>(*this)},
													  {"power"_sv, Action::toMember<Console, &Console::power>(*this)}};

		Screen screen{context_.config.out};
		co_await !screen.setPrompt("> "_sv);
		Suggestions suggestions{entries};

		co_await !processMenu(screen, suggestions);

		co_return {};
	}

	bzd::Async<> processMenu(Screen screen, auto& suggestions)
	{
		auto& in = context_.config.in;

		co_await !screen.printSuggestions(suggestions);

		bzd::Array<bzd::Byte, 1u> buffer;
		auto generator = in.reader(buffer.asBytesMutable());
		auto it = co_await !generator.begin();
		while (it != generator.end())
		{
			auto byte = *bzd::begin(*it);

			// Backspace.
			if (byte == bzd::Byte{127})
			{
				if (!co_await !screen.back())
				{
					break;
				}
			}
			// Tab, autocomplete.
			else if (byte == bzd::Byte{'\t'})
			{
				const auto maybeSuggestion = suggestions.first();
				if (maybeSuggestion.hasValue())
				{
					co_await !screen.set(maybeSuggestion.value());
				}
			}
			else
			{
				co_await !screen.append(byte);
			}

			suggestions.process(screen.getCurrentCommand());
			co_await !screen.printSuggestions(suggestions);

			// There is a single match.
			if (suggestions.size() == 1u && byte != bzd::Byte{127})
			{
				co_await !screen.set(suggestions.first().value());
				co_await !screen.append(static_cast<bzd::Byte>('.'));

				// Select.
				[[maybe_unused]] bzd::Map<bzd::StringView, Output, 4u> entries{
					{"sub1"_sv, Action::toMember<Console, &Console::help>(*this)},
					{"hello"_sv, Action::toMember<Console, &Console::help>(*this)},
					{"nono"_sv, Action::toMember<Console, &Console::help>(*this)},
					{"no"_sv, Action::toMember<Console, &Console::help>(*this)}};
				[[maybe_unused]] Suggestions menu{entries};
				co_await !processMenu(screen.makeNested(), menu);
			}

			co_await !++it;
		}

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
