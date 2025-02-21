#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/threadsafe/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/string_literal.hh"

namespace bzd::io {

template <class Value>
class SourceSetResult : public bzd::threadsafe::RingBufferResult<Value&>
{
private:
	using RingBufferResult = typename bzd::threadsafe::RingBufferResult<Value&>;

public:
	explicit constexpr SourceSetResult(RingBufferResult&& result) noexcept : RingBufferResult{bzd::move(result)} {}
	constexpr SourceSetResult(RingBufferResult&& result, bzd::FunctionRef<void(void)> notify) noexcept :
		RingBufferResult{bzd::move(result)}, notify_{notify}
	{
	}

	SourceSetResult(const SourceSetResult&) = delete;
	SourceSetResult& operator=(const SourceSetResult&) = delete;
	SourceSetResult& operator=(SourceSetResult&&) = delete;
	constexpr SourceSetResult(SourceSetResult&& other) noexcept :
		RingBufferResult{static_cast<RingBufferResult&&>(other)}, notify_{other.notify_}
	{
		other.notify_.reset();
	}

	constexpr ~SourceSetResult() noexcept
	{
		if (notify_.hasValue())
		{
			notify_.value()();
		}
	}

private:
	bzd::Optional<bzd::FunctionRef<void(void)>> notify_{};
};

/// Implementation agnostic class of a source for a specific value type.
template <class Value>
class SourceVirtual
{
public:
	virtual SourceSetResult<Value> trySet() noexcept = 0;
	virtual bzd::Bool trySet(Value&& value) noexcept = 0;
	virtual bzd::Async<SourceSetResult<Value>> set() noexcept = 0;
	virtual bzd::Async<> set(Value&& value) noexcept = 0;
};

template <class Buffer>
class Source : public SourceVirtual<typename Buffer::ValueMutable>
{
private:
	using Value = typename Buffer::ValueMutable;

public:
	constexpr explicit Source(Buffer& buffer) noexcept : buffer_{buffer} {}

public:
	SourceSetResult<Value> trySet() noexcept override
	{
		auto result = buffer_.ring_.nextForWriting();
		if (result)
		{
			// Notify of a new set only after the object destruction.
			return SourceSetResult<Value>{bzd::move(result),
										  bzd::FunctionRef<void(void)>::toMember<Buffer, &Buffer::notifyNewData>(buffer_)};
		}
		return SourceSetResult<Value>{bzd::move(result)};
	}

	bzd::Bool trySet(Value&& value) noexcept override
	{
		if (auto maybeWriter = trySet(); maybeWriter)
		{
			maybeWriter.valueMutable() = bzd::move(value);
			return true;
		}
		return false;
	}

	bzd::Async<SourceSetResult<Value>> set() noexcept override
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

	bzd::Async<> set(Value&& value) noexcept override
	{
		auto writer = co_await !set();
		writer.valueMutable() = bzd::move(value);
		co_return {};
	}

	constexpr StringView getName() const noexcept { return buffer_.getName(); }

private:
	Buffer& buffer_;
};

template <class Value>
class SourceStub : public SourceVirtual<Value>
{
public:
	SourceSetResult<Value> trySet() noexcept override { return bzd::Optional<Value&>{}; }

	bzd::Bool trySet(Value&&) noexcept override { return true; }

	bzd::Async<bzd::Optional<Value&>> set() noexcept override { co_await bzd::Optional<Value&>{}; }

	bzd::Async<> set(Value&&) noexcept override { co_return {}; }

	constexpr StringView getName() const noexcept { return "unset"; }
};

} // namespace bzd::io