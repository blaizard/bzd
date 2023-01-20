#pragma once

#include "cc/bzd/container/threadsafe/ring_buffer.hh"

namespace bzd::io {

template <class Ring>
class Writer
{
public:
	constexpr explicit Writer(Ring& ring) noexcept : ring_{ring} {}

public:
	constexpr auto set() noexcept { return ring_.nextForWriting(); }
	template <class T>
	constexpr bzd::Bool set(T&& value) noexcept
	{
		if (auto maybeWriter = set(); maybeWriter)
		{
			maybeWriter.valueMutable() = bzd::forward<T>(value);
			return true;
		}
		return false;
	}

private:
	Ring& ring_;
};

template <class Ring>
class Reader
{
public:
	constexpr explicit Reader(Ring& ring) noexcept : ring_{ring} {}

public:
	constexpr auto get() noexcept { return ring_.lastForReading(); }
	constexpr auto get(const bzd::Size count) noexcept { return ring_.asSpansForReading(/*count*/ count, /*first*/ false); }

private:
	Ring& ring_;
};

template <class T, Size capacity>
class Buffer
{
private: // Types.
	using Ring = bzd::threadsafe::RingBuffer<T, capacity>;

public: // API.
	constexpr auto makeWriter() noexcept { return bzd::io::Writer<Ring>{ring_}; }
	constexpr auto makeReader() noexcept { return bzd::io::Reader<Ring>{ring_}; }

private:
	Ring ring_{};
};

} // namespace bzd::io
