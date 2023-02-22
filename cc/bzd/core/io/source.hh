#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/threadsafe/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/string_literal.hh"

namespace bzd::io {

template <class Buffer>
class Source
{
private:
	using Value = typename Buffer::ValueMutable;

public:
	constexpr explicit Source(Buffer& buffer) noexcept : buffer_{buffer} {}

public:
	constexpr auto trySet() noexcept
	{
		auto result = buffer_.ring_.nextForWriting();
		if (result)
		{
			buffer_.notifyNewData();
		}
		return result;
	}

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

	constexpr StringView getName() const noexcept { return buffer_.getName(); }

private:
	Buffer& buffer_;
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

	constexpr StringView getName() const noexcept { return "unset"; }
};

} // namespace bzd::io