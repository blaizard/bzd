#pragma once

#include "cc/bzd/algorithm/fill.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

/// Describes the direction to which the stack grows.
enum class StackDirection : UInt8
{
	/// The stack grows from lower address to higher
	upward = 0,
	/// The stack grows from higher address to lower
	downward = 1
};

namespace interface {
template <StackDirection direction = StackDirection::downward>
class Stack
{
public: // Type.
	/// Default value used for tainting the stack.
	static constexpr bzd::Byte defaultTaintPattern{0xaa};

public:
	constexpr Stack(Span<Byte> data) noexcept : data_{data} {}

public:
	constexpr void taint(Byte pattern = defaultTaintPattern) noexcept { bzd::algorithm::fill(data_, pattern); }

	constexpr Byte* data() noexcept { return data_.data(); }

	constexpr Size size() const noexcept { return data_.size(); }

	bzd::Size estimateMaxUsage(Byte pattern = defaultTaintPattern) const noexcept
	{
		if constexpr (direction == StackDirection::downward)
		{
			bzd::Size i{0};
			for (; i < data_.size() && data_[i] == pattern; ++i)
			{
			}
			return data_.size() - i;
		}
		else
		{
			bzd::Size i{data_.size() - 1};
			for (; i > 0 && data_[i] == pattern; --i)
			{
			}
			return i;
		}
	}

private:
	Span<Byte> data_;
};
}

template <bzd::Size stackSize, bzd::Size alignment = 1, StackDirection direction = StackDirection::downward>
class Stack : public interface::Stack<direction>
{
public:
	constexpr Stack() noexcept : interface::Stack<direction>{Span<Byte>{stack_, stackSize}} {}

private:
	alignas(alignment) Byte stack_[stackSize]{};
};

} // namespace bzd
