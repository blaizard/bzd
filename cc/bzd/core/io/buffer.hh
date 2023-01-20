#pragma once

#include "cc/bzd/container/threadsafe/ring_buffer.hh"

namespace bzd::io {

template <class T, Size capacity>
class Buffer
{
private: // Types.
	class Writer
	{
	public:
		constexpr explicit Writer(bzd::threadsafe::RingBuffer<T, capacity>& ring) noexcept : ring_{ring} {}

	public:
		constexpr auto set() noexcept { return ring_.nextForWriting(); }

	private:
		bzd::threadsafe::RingBuffer<T, capacity>& ring_;
	};

	class Reader
	{
	public:
		constexpr explicit Reader(bzd::threadsafe::RingBuffer<T, capacity>& ring) noexcept : ring_{ring} {}

	public:
		constexpr auto get() noexcept { return ring_.lastForReading(); }
		constexpr auto get(const bzd::Size count) noexcept { return ring_.asSpansForReading(/*count*/ count, /*first*/ false); }

	private:
		bzd::threadsafe::RingBuffer<T, capacity>& ring_;
	};

public: // API.
	constexpr auto makeWriter() noexcept { return Writer{ring_}; }

	constexpr auto makeReader() noexcept { return Reader{ring_}; }

private:
	bzd::threadsafe::RingBuffer<T, capacity> ring_;
};

} // namespace bzd::io
