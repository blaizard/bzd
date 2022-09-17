#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

/// Create an output stream buffered implementation.
/// It buffers the output before flushing it.
///
/// \tparam capacityBytes The maximal capacity of the buffer.
template <Size capacityBytes>
class OStreamBuffered : public bzd::OStream
{
public:
	/// Create an output stream buffered object from an existing string.
	constexpr explicit OStreamBuffered(bzd::OStream& stream) noexcept : stream_{stream} {}

	/// Write new data to the buffer, if full the data are automatically flushed.
	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept override
	{
		auto dataLeft = data;
		while (dataLeft.size())
		{
			const auto sizeLeft = buffer_.capacity() - buffer_.size();
			if (sizeLeft < dataLeft.size())
			{
				buffer_.pushBack(dataLeft.first(sizeLeft));
				dataLeft = dataLeft.subSpan(sizeLeft);
				co_await !flush();
			}
			else
			{
				buffer_.pushBack(dataLeft);
				dataLeft = dataLeft.subSpan(dataLeft.size());
			}
		}
		co_return {};
	}

	/// Flush the current data.
	bzd::Async<> flush() noexcept
	{
		co_await !stream_.write(buffer_.asBytes());
		buffer_.clear();
		co_return {};
	}

private:
	bzd::OStream& stream_;
	bzd::Vector<bzd::Byte, capacityBytes> buffer_;
};

} // namespace bzd
