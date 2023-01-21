#pragma once

#include "cc/bzd/container/threadsafe/ring_buffer.hh"
#include "cc/bzd/core/async.hh"

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
private:
	using Value = typename Ring::ValueType;

public:
	constexpr explicit Reader(Ring& ring) noexcept : ring_{ring} {}

public:
	constexpr auto tryGet() noexcept
	{
		auto scope = ring_.lastForReading(/*start*/ index_);
		if (scope)
		{
			index_ = scope.index() + 1u;
		}
		return scope;
	}

	bzd::Async<bzd::threadsafe::RingBufferResult<const Value&>> get() noexcept
	{
		while (true)
		{
			if (auto maybeValue = tryGet(); maybeValue)
			{
				co_return maybeValue;
			}
			co_await bzd::async::yield();
		}
	}

	constexpr auto tryGet(const bzd::Size count) noexcept
	{
		auto scope = ring_.asSpansForReading(/*count*/ count, /*first*/ false, /*start*/ index_);
		if (scope)
		{
			index_ = scope.index() + 1u;
		}
		return scope;
	}

private:
	Ring& ring_;
	bzd::Size index_{0};
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
