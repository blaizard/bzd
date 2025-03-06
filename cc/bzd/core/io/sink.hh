#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/threadsafe/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/string_literal.hh"

namespace bzd::io {

template <class Buffer>
class Sink
{
public:
	using Value = typename Buffer::Value;
	using Index = typename Buffer::Index;

public:
	constexpr explicit Sink(Buffer& buffer) noexcept : buffer_{buffer} {}

public:
	/// Get the last new element for reading and advance the index.
	constexpr auto tryGetNew() noexcept
	{
		auto scope = buffer_.ring_.lastForReading(/*start*/ index_);
		if (scope)
		{
			index_ = scope.index() + 1u;
		}
		return scope;
	}

	/// Get the last X new elements for reading and advance the index.
	constexpr auto tryGetNew(const bzd::Size count) noexcept
	{
		auto scope = buffer_.ring_.asSpansForReading(/*count*/ count, /*first*/ false, /*start*/ index_);
		if (scope)
		{
			index_ = scope.index() + 1u;
		}
		return scope;
	}

	/// Get the last element for reading even if it was already read previously.
	constexpr auto tryGetLast() noexcept { return buffer_.ring_.lastForReading(); }

	/// Wait for a new element to be available and return it.
	bzd::Async<bzd::threadsafe::RingBufferResult<const Value&>> getNew() noexcept
	{
		while (true)
		{
			if (auto maybeValue = tryGetNew(); maybeValue)
			{
				co_return maybeValue;
			}
			co_await !buffer_.waitForData(index_);
		}
	}

	constexpr StringView getName() const noexcept { return buffer_.getName(); }

private:
	Buffer& buffer_;
	Index index_{0};
};

template <class Value>
class SinkStub
{
public:
	constexpr auto tryGetNew() noexcept { return bzd::Optional<const Value&>{}; }

	constexpr auto tryGetNew(const bzd::Size) noexcept { return bzd::Optional<bzd::Spans<const Value, 2u>>{}; }

	constexpr auto tryGetLast() noexcept { return bzd::Optional<const Value&>{}; }

	bzd::Async<bzd::Optional<const Value&>> getNew() noexcept { co_return bzd::Optional<const Value&>{}; }

	constexpr StringView getName() const noexcept { return "unset"; }
};

} // namespace bzd::io
