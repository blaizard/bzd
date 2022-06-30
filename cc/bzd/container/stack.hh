#pragma once

#include "cc/bzd/algorithm/fill.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

/// Describes the direction to which the stack grows.
enum class StackDirection : UInt8
{
	/// The stack grows from lower address to higher
	UPWARD = 0,
	/// The stack grows from higher address to lower
	DOWNWARD = 1
};

template <bzd::Size stackSize, bzd::Size alignment = 1, StackDirection direction = StackDirection::DOWNWARD>
class Stack
{
public: // Type.
	/// Default value used for tainting the stack.
	static constexpr bzd::Byte defaultTaintPattern{0xaa};

public:
	constexpr Stack() noexcept = default;

	constexpr void taint(Byte pattern = defaultTaintPattern) noexcept { bzd::algorithm::fill(stack_, pattern); }

	constexpr Byte* data() noexcept { return stack_.data(); }

	constexpr Size size() const noexcept { return stack_.size(); }

	bzd::Size estimateMaxUsage(Byte pattern = defaultTaintPattern) const noexcept
	{
		if constexpr (direction == StackDirection::DOWNWARD)
		{
			bzd::Size i{0};
			for (; i < stack_.size() && stack_[i] == pattern; ++i)
			{
			}
			return stack_.size() - i;
		}
		else
		{
			bzd::Size i{stack_.size() - 1};
			for (; i > 0 && stack_[i] == pattern; --i)
			{
			}
			return i;
		}
	}

private:
	alignas(alignment) bzd::Array<Byte, stackSize> stack_{};
};

} // namespace bzd
