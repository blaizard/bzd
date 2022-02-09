#pragma once

#include "cc/bzd.hh"

namespace bzd {

template <bzd::SizeType N>
class Reader
{
public: // Constructor.
	constexpr Reader(bzd::IStream& in,
					 bzd::OStream& out,
					 bzd::Span<const bzd::ByteType> separators = bzd::Span<const bzd::ByteType>{}) noexcept :
		in_{in},
		out_{out}, separators_{separators}
	{
	}

public: // API.
	/// Read a sequence of bytes until the specified stop byte and return what has been read.
	/// This function is statefull and if additional bytes are read, they will not be lost
	/// and re-used for the next call.
	///
	/// \param stop The special byte until which the read operation will stop.
	///
	/// \return A span containing the result including the stop character.
	[[nodiscard]] bzd::Async<bzd::Spans<const bzd::ByteType, 2>> readUntil(const bzd::ByteType stop) noexcept
	{
		while (true)
		{
			// Read some data from the input stream.
			auto maybeResult = co_await produce();
			ASSERT_ASYNC_RESULT(maybeResult);
			auto result = bzd::move(maybeResult.value());

			// Check if the special byte appear.
			if (const auto pos = result.find(stop); pos != bzd::npos)
			{
				auto sub = buffer_.asSpans().subSpans(0, buffer_.size() - (result.size() - pos) + 1);
				buffer_.consume(sub.size());
				co_return sub;
			}
		}
	}

	/// Read any of the given string, if no match, return an error.
	template <class Key>
	[[nodiscard]] bzd::Async<Key> readAnyOf(const bzd::interface::Map<bzd::StringView, Key>& keywords) noexcept
	{
		using Element = typename bzd::interface::Map<bzd::StringView, Key>::Element;

		auto first = keywords.begin();
		auto last = keywords.end();
		bzd::SizeType index{0};

		while (true)
		{
			// Read some data from the input stream.
			auto maybeResult = co_await produce();
			ASSERT_ASYNC_RESULT(maybeResult);
			auto result = bzd::move(maybeResult.value());

			for (const auto c : result)
			{
				buffer_.consume(1);

				if (bzd::algorithm::find(separators_.begin(), separators_.end(), c) != separators_.end())
				{
					// Ignore separators at the begining of the data.
					if (!index)
					{
						continue;
					}

					// Find the element that matches from the size point of view. There must be only one
					// as map cannot hold duplicated keys.
					first = bzd::algorithm::findIf(first, last, [&index](const Element& elt) { return (elt.first.size() == index); });
					last = (first == last) ? first : (first + 1);
				}
				else
				{
					first = bzd::algorithm::lowerBound(first, last, c, [&index](const Element& elt, const bzd::ByteType c) {
						return index >= elt.first.size() || elt.first.at(index) < static_cast<char>(c);
					});
					last = bzd::algorithm::upperBound(first, last, c, [&index](const bzd::ByteType c, const Element& elt) {
						return index >= elt.first.size() || static_cast<char>(c) < elt.first.at(index);
					});
					++index;
				}

				// Print the result
				if (first == last)
				{
					co_return bzd::error(bzd::ErrorType::failure, CSTR("No match."));
				}
				else if (bzd::iterator::distance(first, last) == 1 && first->first.size() == index)
				{
					co_return first->second;
				}

				std::cout << "Match(es): (index=" << index << ") ";
				for (auto it = first; it != last; ++it)
				{
					std::cout << it->first.data() << ":" << it->second << " ";
				}
				std::cout << std::endl;
			}
		}

		co_return Key{};
	}

	/// Reset the internal buffer, loosing the currently stored data if any.
	constexpr void clear() noexcept { buffer_.clear(); }

private:
	/// Produces new data in the ring buffer, read from the input stream.
	[[nodiscard]] bzd::Async<bzd::Span<bzd::ByteType>> produce() noexcept
	{
		auto span = buffer_.asSpanForWriting();
		if (span.empty())
		{
			co_return bzd::error(bzd::ErrorType::failure, CSTR("Ring buffer of {} bytes is full."), buffer_.size());
		}
		auto maybeResult = co_await in_.read(span);
		ASSERT_ASYNC_RESULT(maybeResult);

		auto result = bzd::move(maybeResult.value());
		buffer_.produce(result.size());

		// Replace 'del' == 0x7f with '\b' <- back one char.
		// Echo what is being typed
		auto writeResult = co_await out_.write(result);
		ASSERT_ASYNC_RESULT(writeResult);

		co_return result;
	}

private: // Variables.
	bzd::IStream& in_;
	bzd::OStream& out_;
	bzd::Span<const bzd::ByteType> separators_;
	bzd::RingBuffer<bzd::ByteType, N> buffer_{};
};

} // namespace bzd
