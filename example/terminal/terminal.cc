#include "cc/bzd.hh"

namespace Example {

template <bzd::SizeType N>
class Reader
{
public: // Constructor.
	constexpr Reader(bzd::IStream& in, bzd::OStream& out) noexcept : in_{in}, out_{out} {}

public: // API.
	/// Read a sequence of bytes until a certain byte and return what has been read.
	/// This function is statefull and if additional bytes have been read, they will not be lost
	/// and re-used for the next call.
	///
	/// \param stop The special byte until which the read will stop.
	[[nodiscard]] bzd::Async<bzd::Span<bzd::ByteType>> readUntil(const bzd::ByteType stop) noexcept
	{
		auto readSpan = buffer_.asSpan();

		// If there was a previous read, move the last position not to loose previously read bytes.
		if (!left_.empty())
		{
			bzd::algorithm::copy(left_.begin(), left_.end(), buffer_.begin());
			readSpan = readSpan.subSpan(left_.size());
		}

		while (true)
		{
			const auto result = co_await in_.read(readSpan);
			if (!result)
			{
				co_return bzd::error();
			}
			// Echo what is being typed
			co_await out_.write(result.value());
			// Check if the special byte appear.
			if (const auto pos = result->find(stop); pos != bzd::npos)
			{
				const auto size = bzd::iterator::distance(buffer_.begin(), readSpan.begin() + pos) + 1;
				left_ = buffer_.subSpan(size, result->size() - pos);
				co_return buffer_.subSpan(0, size);
			}
			// Move the read span forward
			readSpan = readSpan.subSpan(result->size());
		}
	}

private: // Variables.
	bzd::IStream& in_;
	bzd::OStream& out_;
	bzd::Span<bzd::ByteType> left_{};
	bzd::Array<bzd::ByteType, N> buffer_{};
};

bzd::Async<bool> run()
{
	Reader<10> reader{bzd::platform::in(), bzd::platform::out()};

	for (int i = 0; i < 5; ++i)
	{
		const auto result = co_await reader.readUntil(bzd::ByteType{'\n'});
		if (result)
		{
			co_await bzd::platform::out().write(result.value());
		}
		else
		{
			co_await bzd::log::error(CSTR("Error while reading."));
		}
	}

	co_return true;
}

} // namespace Example
