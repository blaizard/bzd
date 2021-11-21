#include "cc/bzd.hh"

namespace Example {

template <bzd::SizeType N>
class Reader
{
public: // Constructor.
	constexpr Reader(bzd::IStream& in, bzd::OStream& out) noexcept : in_{in}, out_{out} {}

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
			auto span = buffer_.asSpanForWriting();
			auto maybeResult = co_await in_.read(span);
			if (!maybeResult)
			{
				co_return maybeResult.propagate();
			}
			auto result = bzd::move(maybeResult.value());
			buffer_.produce(result.size());

			// Replace 'del' == 0x7f with '\b' <- back one char.
			// Echo what is being typed
			co_await out_.write(result);

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
	[[nodiscard]] bzd::Async<bzd::SizeType> readAnyOf() noexcept { co_return 0; }

	/// Reset the internal buffer, loosing the currently stored data if any.
	constexpr void clear() noexcept { buffer_.clear(); }

private: // Variables.
	bzd::IStream& in_;
	bzd::OStream& out_;
	bzd::RingBuffer<bzd::ByteType, N> buffer_{};
};

bzd::Async<bool> run()
{
	Reader<16> reader{bzd::platform::in(), bzd::platform::out()};

	for (int i = 0; i < 5; ++i)
	{
		const auto result = co_await reader.readUntil(bzd::ByteType{'\n'});
		if (result)
		{
			co_await bzd::platform::out().write(result.value());
		}
		else
		{
			co_await bzd::log::error(CSTR("Error while reading, reseting state."));
			reader.clear();
		}
	}

	co_return true;
}

} // namespace Example
