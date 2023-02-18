#pragma once

#include "cc/bzd/container/threadsafe/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/string_literal.hh"

namespace bzd::io {

template <class Ring, meta::StringLiteral identifier>
class Source
{
private:
	using Value = typename Ring::ValueMutableType;

public:
	constexpr explicit Source(Ring& ring) noexcept : ring_{ring} {}

public:
	constexpr auto trySet() noexcept { return ring_.nextForWriting(); }

	template <class T>
	constexpr bzd::Bool trySet(T&& value) noexcept
	{
		if (auto maybeWriter = trySet(); maybeWriter)
		{
			maybeWriter.valueMutable() = bzd::forward<T>(value);
			return true;
		}
		return false;
	}

	bzd::Async<bzd::threadsafe::RingBufferResult<Value&>> set() noexcept
	{
		while (true)
		{
			if (auto maybeValue = trySet(); maybeValue)
			{
				co_return maybeValue;
			}
			co_await bzd::async::yield();
		}
	}

	template <class T>
	bzd::Async<> set(T&& value) noexcept
	{
		auto writer = co_await !set();
		writer.valueMutable() = bzd::forward<T>(value);
		co_return {};
	}

private:
	Ring& ring_;
};

template <class Value>
class SourceStub
{
public:
	constexpr auto trySet() noexcept { return bzd::Optional<Value&>{}; }

	template <class T>
	constexpr bzd::Bool trySet(T&&) noexcept
	{
		return true;
	}

	bzd::Async<bzd::Optional<Value&>> set() noexcept { co_await bzd::Optional<Value&>{}; }

	template <class T>
	bzd::Async<> set(T&&) noexcept
	{
		co_return {};
	}
};

template <class Ring, meta::StringLiteral identifier>
class Sink
{
public:
	using Value = typename Ring::ValueType;

public:
	constexpr explicit Sink(Ring& ring) noexcept : ring_{ring} {}

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

	constexpr StringView getName() const noexcept { return StringView{identifier.data(), identifier.size()}; }

private:
	Ring& ring_;
	bzd::Size index_{0};
};

template <class Value>
class SinkStub
{
public:
	constexpr auto tryGet() noexcept { return bzd::Optional<const Value&>{}; }

	bzd::Async<bzd::Optional<const Value&>> get() noexcept { co_return bzd::Optional<const Value&>{}; }

	constexpr auto tryGet(const bzd::Size) noexcept { return bzd::Optional<bzd::Spans<const Value, 2u>>{}; }
};

template <class T, Size capacity, meta::StringLiteral identifier>
class Buffer
{
private: // Types.
	using Ring = bzd::threadsafe::RingBuffer<T, capacity>;

public: // API.
	constexpr auto makeSource() noexcept { return bzd::io::Source<Ring, identifier>{ring_}; }
	constexpr auto makeSink() noexcept { return bzd::io::Sink<Ring, identifier>{ring_}; }

private:
	Ring ring_{};
};

} // namespace bzd::io
