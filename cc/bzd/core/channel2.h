#pragma once

#include "cc/bzd/container/promise.h"
#include "cc/bzd/container/span.h"
#include "cc/bzd/platform/types.h"

/*
	[[nodiscard]] bzd::Promise<bzd::SizeType> write(const bzd::Span<const bzd::ByteType>& data) noexcept;
	[[nodiscard]] bzd::Promise<bzd::SizeType> write(const bzd::Span<bzd::ByteType>& data) noexcept
	{
		return write(reinterpret_cast<const bzd::Span<const bzd::ByteType>&>(data));
	}
	bzd::Promise<bzd::SizeType> write(bzd::Span<const bzd::ByteType>&& data) noexcept = delete;
	bzd::Promise<bzd::SizeType> write(bzd::Span<bzd::ByteType>&& data) noexcept = delete;

	[[nodiscard]] bzd::Promise<bzd::SizeType> read(const bzd::Span<bzd::ByteType>& data) noexcept;
	bzd::Promise<bzd::SizeType> read(bzd::Span<bzd::ByteType>&& data) noexcept = delete;
*/

namespace bzd {

class OChannel
{
public:
	/**
	 * Write data to an output channel.
	 *
	 * The promise resolves only after all the data is transmitted.
	 *
	 * \param data The data to be sent via this output channel.
	 *
	 * \return A promise resolving when all the data have been transmitted.
	 */
	virtual [[nodiscard]] Promise<> write(const Span<const ByteType>& data) noexcept = 0;
	Promise<> write(Span<const ByteType>&& data) noexcept = delete;
};

class IChannel
{
public:
	/**
	 * Read data from an input channel.
	 *
	 * The promise resolves only after at least one byte is received.
	 *
	 * \param data The input buffer to store the data received.
	 *
	 * \return A promise resolving as the number of bytes received.
	 */
	virtual [[nodiscard]] Promise<SizeType> read(const Span<ByteType>& data) noexcept = 0;

	[[nodiscard]] constexpr Promise<SizeType> readAll(const Span<ByteType>& data) noexcept
	{
		Span<ByteType> buffer{data};
		while (buffer.size())
		{
			const auto result = await read(buffer);
			if (!result)
			{
				return result;
			}
			buffer = buffer.subSpan(*result);
		}
	}

	Promise<SizeType> read(Span<ByteType>&& data) noexcept = delete;
};

class IOChannel
	: public IChannel
	, public OChannel
{
};
} // namespace bzd
